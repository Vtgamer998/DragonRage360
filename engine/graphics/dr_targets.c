/* SPDX-License-Identifier: AGPL-3.0-or-later
 * GPU contracts: xsdk revision 57b02f1e60667e37d22c3aa613d23d2486514ff8,
 * include/xsdk/gpu.h, texture.h, tests/test_depth.c. Shader source is our GLSL;
 * no proprietary binaries. GPU execution of this module is unverified.
 */
#include "dr_targets.h"
#include "dr_xenos.h"
#include <stddef.h>
#include "xsdk/gpu.h"
#include "dr_restore_shaders.h"
#include "dr_gs_targets_shaders.h"
#include <string.h>
enum { COUNT=24,BUDGET=128*1024*1024 };
static struct {
    int opened,current;
    uint32_t bytes,fence;
    XsdkShader vertex,pixel;
    XsdkSamplerState sampler;
    volatile uint32_t *quad;
    struct {XsdkTexture color,depth,aux,depth_bytes;uint32_t bytes;int made,valid,gs;} slots[COUNT];
    int gs_open;
    XsdkShader gs_restore,stencil;
    XsdkCommandBuffer recording;
    XsdkTexture snapshot[2];
    uint32_t snapshot_width[2],snapshot_height[2],snapshot_id[2];
} pool;
static uint64_t allocation_bytes(const XsdkTexture *t)
{return ((uint64_t)t->size_bytes+65535u)&~(uint64_t)65535u;}
static int wait_idle(void)
{
    if(pool.fence && !NT_SUCCESS(xsdk_gpu_wait(pool.fence,1000))) return DR_XENOS_GPU;
    pool.fence=0;return 0;
}
int dr_targets_open(void)
{
    if(pool.opened || pool.quad) return DR_XENOS_STATE;
    pool.current=-1;
    NTSTATUS s=xsdk_shader_load(&pool.vertex,dr_restore_vert.container,dr_restore_vert.container_bytes);
    if(NT_SUCCESS(s)) s=xsdk_shader_load(&pool.pixel,dr_restore_frag.container,dr_restore_frag.container_bytes);
    const XsdkVertexElement layout[]={
        {.usage=XSDK_USAGE_POSITION,.format=XSDK_VERTEX_FLOAT4,.offset_bytes=0},
        {.usage=XSDK_USAGE_TEXCOORD,.format=XSDK_VERTEX_FLOAT2,.offset_bytes=16}};
    if(NT_SUCCESS(s)) s=xsdk_shader_set_vertex_layout(&pool.vertex,layout,2,(const uint32_t[]){24});
    if(NT_SUCCESS(s)) pool.quad=xsdk_gpu_alloc(6*24);
    if(!NT_SUCCESS(s) || !pool.quad) {
        xsdk_shader_free(&pool.pixel);xsdk_shader_free(&pool.vertex);
        memset(&pool,0,sizeof(pool));return DR_XENOS_GPU;
    }
    const float vertices[]={-1,1,0,1,0,0, 1,1,0,1,1,0, -1,-1,0,1,0,1,
                            -1,-1,0,1,0,1, 1,1,0,1,1,0, 1,-1,0,1,1,1};
    for(unsigned i=0;i<sizeof(vertices)/4;i++) {
        uint32_t word;memcpy(&word,&vertices[i],4);pool.quad[i]=word;
    }
    xsdk_gpu_invalidate(xsdk_gpu_address(pool.quad),sizeof(vertices));
    const XsdkSampler point={.mag_filter=XSDK_FILTER_POINT,.min_filter=XSDK_FILTER_POINT,
        .mip_filter=XSDK_FILTER_POINT,.address_u=XSDK_ADDRESS_CLAMP,.address_v=XSDK_ADDRESS_CLAMP};
    xsdk_texture_sampler_state(&pool.sampler,&point);pool.opened=1;return 0;
}
int dr_targets_size(unsigned id,uint32_t *w,uint32_t *h)
{
    if(!pool.opened) return DR_XENOS_STATE;
    if(id>=COUNT || !pool.slots[id].made || !w || !h) return DR_XENOS_INVALID;
    *w=pool.slots[id].color.width;*h=pool.slots[id].color.height;return 0;
}
int dr_targets_ensure(unsigned id,uint32_t w,uint32_t h)
{
    if(!pool.opened || pool.current>=0) return DR_XENOS_STATE;
    if(id>=COUNT || !w || !h || (w&31) || w>1280 || h>1024 ||
       xsdk_gpu_target_tiles(w,h)>XSDK_EDRAM_TILES/2) return DR_XENOS_INVALID;
    if(pool.slots[id].made && pool.slots[id].gs) return DR_XENOS_STATE;
    if(pool.slots[id].made && pool.slots[id].color.width==w && pool.slots[id].color.height==h) return 0;
    int error=wait_idle();if(error) return error;
    XsdkTexture color={0},depth={0};
    NTSTATUS s=xsdk_texture_init(&color,XSDK_FORMAT_R8G8B8A8,w,h,1,true);
    if(NT_SUCCESS(s)) s=xsdk_texture_init(&depth,XSDK_FORMAT_D24S8,w,h,1,true);
    if(!NT_SUCCESS(s)) return DR_XENOS_GPU;
    uint64_t added=allocation_bytes(&color)+allocation_bytes(&depth);
    /* Preflight actual SDK layouts INCLUDING page rounding and the old slot.
     * Nothing is allocated before the peak budget is checked. */
    if(added>BUDGET || added+pool.bytes>BUDGET) return DR_XENOS_FULL;
    s=xsdk_texture_create(&color,XSDK_FORMAT_R8G8B8A8,w,h,1,true);
    if(!NT_SUCCESS(s)) return DR_XENOS_GPU;
    s=xsdk_texture_create(&depth,XSDK_FORMAT_D24S8,w,h,1,true);
    if(!NT_SUCCESS(s)) {xsdk_texture_free(&color);return DR_XENOS_GPU;}
    xsdk_texture_base_at_corner(&color);xsdk_texture_base_at_corner(&depth);
    if(added!=allocation_bytes(&color)+allocation_bytes(&depth)) {
        xsdk_texture_free(&depth);xsdk_texture_free(&color);return DR_XENOS_GPU;
    }
    if(pool.slots[id].made) {
        pool.bytes-=pool.slots[id].bytes;
        xsdk_texture_free(&pool.slots[id].depth);xsdk_texture_free(&pool.slots[id].color);
    }
    pool.slots[id].color=color;pool.slots[id].depth=depth;
    pool.slots[id].made=1;pool.slots[id].valid=0;pool.slots[id].bytes=(uint32_t)added;
    pool.bytes+=(uint32_t)added;return 0;
}
static int gs_open(void)
{
    if(pool.gs_open) return 0;
    XsdkTexture layout;
    if(!NT_SUCCESS(xsdk_texture_init(&layout,XSDK_FORMAT_R8G8B8A8,1280,1024,1,true))) return DR_XENOS_GPU;
    if(pool.bytes+2*allocation_bytes(&layout)>BUDGET) return DR_XENOS_FULL;
    NTSTATUS s=xsdk_shader_load(&pool.gs_restore,dr_gs_restore_frag.container,dr_gs_restore_frag.container_bytes);
    if(NT_SUCCESS(s)) s=xsdk_shader_load(&pool.stencil,dr_gs_stencil_frag.container,dr_gs_stencil_frag.container_bytes);
    if(NT_SUCCESS(s)) s=xsdk_gpu_command_buffer_create(&pool.recording,262144);
    /* Shared snapshots cover the largest supported target, allocated outside drawing. */
    if(NT_SUCCESS(s)) s=xsdk_texture_create(&pool.snapshot[0],XSDK_FORMAT_R8G8B8A8,1280,1024,1,true);
    if(NT_SUCCESS(s)) s=xsdk_texture_create(&pool.snapshot[1],XSDK_FORMAT_R8G8B8A8,1280,1024,1,true);
    if(!NT_SUCCESS(s)) {
        if(pool.snapshot[1].texels) xsdk_texture_free(&pool.snapshot[1]);
        if(pool.snapshot[0].texels) xsdk_texture_free(&pool.snapshot[0]);
        if(pool.recording.dwords) xsdk_gpu_command_buffer_free(&pool.recording);
        xsdk_shader_free(&pool.stencil);xsdk_shader_free(&pool.gs_restore);
        memset(pool.snapshot,0,sizeof(pool.snapshot));memset(&pool.recording,0,sizeof(pool.recording));
        return DR_XENOS_GPU;
    }
    xsdk_texture_base_at_corner(&pool.snapshot[0]);xsdk_texture_base_at_corner(&pool.snapshot[1]);
    pool.bytes+=(uint32_t)(allocation_bytes(&pool.snapshot[0])+allocation_bytes(&pool.snapshot[1]));
    pool.gs_open=1;return 0;
}
int dr_targets_is_gs(unsigned id)
{return pool.opened && id<COUNT && pool.slots[id].made && pool.slots[id].gs;}
int dr_targets_ready(unsigned id)
{return dr_targets_is_gs(id) && pool.slots[id].valid && pool.current!=(int)id;}
const XsdkTexture *dr_targets_texture(unsigned id,unsigned attachment)
{
    if(!pool.opened || id>=COUNT || !pool.slots[id].made || attachment>2 ||
       (attachment==1 && !pool.slots[id].gs)) return NULL;
    return attachment==0?&pool.slots[id].color:attachment==1?&pool.slots[id].aux:&pool.slots[id].depth;
}
int dr_targets_ensure_gs(unsigned id,uint32_t w,uint32_t h)
{
    if(!pool.opened || pool.current>=0) return DR_XENOS_STATE;
    if(id>=COUNT || !w || !h || (w&31) || w>1280 || h>1024) return DR_XENOS_INVALID;
    if(pool.slots[id].made && !pool.slots[id].gs) return DR_XENOS_STATE;
    if(pool.slots[id].made && pool.slots[id].color.width==w && pool.slots[id].color.height==h) return 0;
    int error=wait_idle();if(error) return error;
    error=gs_open();if(error) return error;
    XsdkTexture color={0},depth={0},aux={0},view={0};
    NTSTATUS s=xsdk_texture_init(&color,XSDK_FORMAT_R8G8B8A8,w,h,1,true);
    if(NT_SUCCESS(s)) s=xsdk_texture_init(&depth,XSDK_FORMAT_D24S8,w,h,1,true);
    if(NT_SUCCESS(s)) s=xsdk_texture_init(&aux,XSDK_FORMAT_R8G8B8A8,w,h,1,true);
    if(!NT_SUCCESS(s)) return DR_XENOS_GPU;
    uint64_t added=allocation_bytes(&color)+allocation_bytes(&depth)+allocation_bytes(&aux);
    if(added+pool.bytes>BUDGET) return DR_XENOS_FULL;
    s=xsdk_texture_create(&color,XSDK_FORMAT_R8G8B8A8,w,h,1,true);
    if(NT_SUCCESS(s)) s=xsdk_texture_create(&depth,XSDK_FORMAT_D24S8,w,h,1,true);
    if(NT_SUCCESS(s)) s=xsdk_texture_create(&aux,XSDK_FORMAT_R8G8B8A8,w,h,1,true);
    if(!NT_SUCCESS(s)) {
        if(aux.texels) xsdk_texture_free(&aux);
        if(depth.texels) xsdk_texture_free(&depth);
        if(color.texels) xsdk_texture_free(&color);
        return DR_XENOS_GPU;
    }
    xsdk_texture_base_at_corner(&color);xsdk_texture_base_at_corner(&depth);xsdk_texture_base_at_corner(&aux);
    if(added!=allocation_bytes(&color)+allocation_bytes(&depth)+allocation_bytes(&aux)) {
        xsdk_texture_free(&aux);xsdk_texture_free(&depth);xsdk_texture_free(&color);return DR_XENOS_GPU;
    }
    s=xsdk_texture_init(&view,XSDK_FORMAT_R8G8B8A8,w,h,1,true);
    if(!NT_SUCCESS(s) || view.size_bytes!=depth.size_bytes || view.pitch!=depth.pitch) {
        xsdk_texture_free(&aux);xsdk_texture_free(&depth);xsdk_texture_free(&color);return DR_XENOS_GPU;
    }
    xsdk_texture_base_at_corner(&view);xsdk_texture_attach(&view,depth.texels);
    if(pool.slots[id].made) {
        pool.bytes-=pool.slots[id].bytes;
        xsdk_texture_free(&pool.slots[id].aux);xsdk_texture_free(&pool.slots[id].depth);xsdk_texture_free(&pool.slots[id].color);
    }
    pool.slots[id].color=color;pool.slots[id].depth=depth;pool.slots[id].aux=aux;pool.slots[id].depth_bytes=view;
    for(unsigned i=0;i<2;i++) if(pool.snapshot_id[i]==id) pool.snapshot_width[i]=0;
    pool.slots[id].bytes=(uint32_t)added;pool.slots[id].made=1;pool.slots[id].gs=1;pool.slots[id].valid=0;
    pool.bytes+=(uint32_t)added;return 0;
}
static int gs_store(int snapshot)
{
    unsigned id=(unsigned)pool.current;
    XsdkTileResolve resolves[4]={
        {.target=XSDK_TILE_DEPTH,.texture=&pool.slots[id].depth,.drawn_back=pool.slots[id].valid!=0},
        {.target=0,.texture=&pool.slots[id].color,.drawn_back=pool.slots[id].valid!=0},
        {.target=1,.texture=&pool.slots[id].aux,.drawn_back=pool.slots[id].valid!=0}};
    XsdkTexture view;
    if(snapshot>=0) {
        view=pool.snapshot[snapshot];
        view.width=pool.slots[id].color.width;view.height=pool.slots[id].color.height;
        xsdk_texture_base_at_corner(&view);
        /* Keep the snapshot's allocated pitch: sampling divides by its full extent. */
        resolves[3]=(XsdkTileResolve){.target=1,.texture=&view,.drawn_back=true};
    }
    NTSTATUS status=xsdk_gpu_end_tiles(resolves,snapshot<0?3:4);
    pool.current=-1;pool.fence=xsdk_gpu_fence();
    if(!NT_SUCCESS(status)) return DR_XENOS_GPU;
    pool.slots[id].valid=1;
    if(snapshot>=0) {
        pool.snapshot_width[snapshot]=pool.slots[id].color.width;
        pool.snapshot_height[snapshot]=pool.slots[id].color.height;
        pool.snapshot_id[snapshot]=id;
    }
    return 0;
}
int dr_targets_store(void)
{
    if(!pool.opened || pool.current<0) return DR_XENOS_STATE;
    if(pool.slots[pool.current].gs) return gs_store(-1);
    unsigned id=(unsigned)pool.current;
    /* Save depth before color clears eDRAM. Resolve mutates the drawing state. */
    xsdk_gpu_set_resolve_clears(false,false);
    xsdk_gpu_resolve_depth_texture(&pool.slots[id].depth);
    /* Resolve invalidates the target/shaders. Re-establish the same eDRAM layout
     * before the second resolve; set_target does not clear its pixels. */
    xsdk_gpu_set_target(pool.slots[id].color.width,pool.slots[id].color.height);
    xsdk_gpu_set_depth_clear(0,0);
    xsdk_gpu_set_resolve_clears(true,true);
    xsdk_gpu_resolve_texture(&pool.slots[id].color,0);
    pool.slots[id].valid=1;pool.current=-1;pool.fence=xsdk_gpu_fence();return 0;
}
int dr_targets_bind(unsigned id)
{
    if(!pool.opened) return DR_XENOS_STATE;
    if(id>=COUNT || !pool.slots[id].made) return DR_XENOS_INVALID;
    if(pool.current==(int)id) return 0;
    if(pool.current>=0) {int error=dr_targets_store();if(error) return error;}
    if(pool.slots[id].gs) {
        int error=wait_idle();if(error) return error;
        const XsdkTiling tiling={.width=pool.slots[id].color.width,.height=pool.slots[id].color.height,
            .samples=1,.count=2,.targets={{.format=XSDK_TARGET_8_8_8_8},{.format=XSDK_TARGET_8_8_8_8}},.depth=true};
        xsdk_gpu_set_depth_clear(0,0);
        if(!NT_SUCCESS(xsdk_gpu_begin_tiles(&tiling,&pool.recording))) return DR_XENOS_GPU;
        pool.current=(int)id;
        if(pool.slots[id].valid) {
            const XsdkDepthStencil depth={.depth_test=true,.depth_write=true,.depth_compare=XSDK_COMPARE_ALWAYS};
            XsdkDepthStencilState state;xsdk_gpu_depth_stencil_state(&state,&depth);xsdk_gpu_set_depth_stencil(&state);
            const XsdkBlend plain={.color_source=XSDK_BLEND_ONE,.color_dest=XSDK_BLEND_ZERO,.color_op=XSDK_BLEND_OP_ADD,
                .alpha_source=XSDK_BLEND_ONE,.alpha_dest=XSDK_BLEND_ZERO,.alpha_op=XSDK_BLEND_OP_ADD,.write_mask=15};
            XsdkBlendState blends[2];xsdk_gpu_blend_state(&blends[0],&plain);blends[1]=blends[0];xsdk_gpu_set_blends(blends,2);
            xsdk_gpu_set_shaders(&pool.vertex,&pool.gs_restore);
            xsdk_built_shader_set_literals(&dr_restore_vert);xsdk_built_shader_set_literals(&dr_gs_restore_frag);
            xsdk_gpu_set_texture(DR_GS_RESTORE_FRAG_SAMPLER_0_0,&pool.slots[id].color,&pool.sampler);
            xsdk_gpu_set_texture(DR_GS_RESTORE_FRAG_SAMPLER_0_1,&pool.slots[id].depth,&pool.sampler);
            xsdk_gpu_set_texture(DR_GS_RESTORE_FRAG_SAMPLER_0_2,&pool.slots[id].aux,&pool.sampler);
            xsdk_gpu_set_vertices(0,xsdk_gpu_address(pool.quad),6*24);xsdk_gpu_draw(XSDK_PRIMITIVE_TRIANGLE_LIST,6);
            /* Restore every stencil bit from the raw resolved word, never from sampled depth. */
            XsdkBlend masked=plain;masked.write_mask=0;
            xsdk_gpu_blend_state(&blends[0],&masked);blends[1]=blends[0];xsdk_gpu_set_blends(blends,2);
            xsdk_gpu_set_shaders(&pool.vertex,&pool.stencil);
            xsdk_built_shader_set_literals(&dr_restore_vert);xsdk_built_shader_set_literals(&dr_gs_stencil_frag);
            xsdk_gpu_set_texture(DR_GS_STENCIL_FRAG_SAMPLER_0_0,&pool.slots[id].depth_bytes,&pool.sampler);
            for(unsigned pass=0;pass<=8;pass++) {
                uint8_t bit=pass?(uint8_t)(1u<<(pass-1)):0;
                const XsdkDepthStencil stencil={.depth_test=true,.depth_compare=XSDK_COMPARE_ALWAYS,
                    .stencil_test=true,.stencil_compare=XSDK_COMPARE_ALWAYS,.stencil_fail=XSDK_STENCIL_KEEP,
                    .stencil_depth_fail=XSDK_STENCIL_KEEP,.stencil_pass=XSDK_STENCIL_REPLACE,
                    .stencil_reference=bit,.stencil_read_mask=255,.stencil_write_mask=pass?bit:255};
                xsdk_gpu_depth_stencil_state(&state,&stencil);xsdk_gpu_set_depth_stencil(&state);
                const float params[]={(float)bit,0,0,0};
                xsdk_gpu_set_pixel_constants(DR_GS_STENCIL_FRAG_PUSH,params,DR_GS_STENCIL_FRAG_PUSH_REGISTERS);
                xsdk_gpu_draw(XSDK_PRIMITIVE_TRIANGLE_LIST,6);
            }
        }
        return 0;
    }
    const XsdkTexture *color=&pool.slots[id].color;
    xsdk_gpu_set_target(color->width,color->height);
    xsdk_gpu_set_depth_clear(0,0);
    if(!pool.slots[id].valid) {
        /* Discard the undefined initial image. Resolve clears eDRAM to zero;
         * the copied undefined texture is not sampled and is overwritten by store. */
        xsdk_gpu_set_resolve_clears(true,true);
        xsdk_gpu_resolve_texture(color,0);
        xsdk_gpu_set_target(color->width,color->height);
    } else {
        const XsdkDepthStencil depth={.depth_test=true,.depth_write=true,.depth_compare=XSDK_COMPARE_ALWAYS};
        XsdkDepthStencilState state;xsdk_gpu_depth_stencil_state(&state,&depth);xsdk_gpu_set_depth_stencil(&state);
        xsdk_gpu_set_shaders(&pool.vertex,&pool.pixel);
        xsdk_built_shader_set_literals(&dr_restore_vert);xsdk_built_shader_set_literals(&dr_restore_frag);
        xsdk_gpu_set_texture(DR_RESTORE_FRAG_SAMPLER_0_0,color,&pool.sampler);
        xsdk_gpu_set_texture(DR_RESTORE_FRAG_SAMPLER_0_1,&pool.slots[id].depth,&pool.sampler);
        xsdk_gpu_set_vertices(0,xsdk_gpu_address(pool.quad),6*24);
        xsdk_gpu_draw(XSDK_PRIMITIVE_TRIANGLE_LIST,6);
    }
    pool.current=(int)id;
    /* Covers a restore draw even if a later store has not happened yet. */
    pool.fence=xsdk_gpu_fence();return 0;
}
int dr_targets_snapshot(unsigned kind)
{
    if(kind>1 || pool.current<0 || !pool.slots[pool.current].gs) return DR_XENOS_STATE;
    unsigned id=(unsigned)pool.current;
    int error=gs_store((int)kind);return error?error:dr_targets_bind(id);
}
const XsdkTexture *dr_targets_snapshot_texture(unsigned kind,unsigned id,uint32_t w,uint32_t h)
{
    if(kind>1 || !pool.gs_open || pool.snapshot_id[kind]!=id || pool.snapshot_width[kind]!=w || pool.snapshot_height[kind]!=h) return NULL;
    return &pool.snapshot[kind];
}
int dr_targets_drop(unsigned id)
{
    if(!pool.opened || pool.current>=0) return DR_XENOS_STATE;
    if(id>=COUNT) return DR_XENOS_INVALID;
    if(!pool.slots[id].made) return 0;
    int error=wait_idle();if(error) return error;
    for(unsigned i=0;i<2;i++) if(pool.snapshot_id[i]==id) pool.snapshot_width[i]=0;
    pool.bytes-=pool.slots[id].bytes;
    if(pool.slots[id].gs) xsdk_texture_free(&pool.slots[id].aux);
    xsdk_texture_free(&pool.slots[id].depth);xsdk_texture_free(&pool.slots[id].color);
    memset(&pool.slots[id],0,sizeof(pool.slots[id]));return 0;
}
int dr_targets_close(void)
{
    if(!pool.opened || pool.current>=0) return DR_XENOS_STATE;
    int error=wait_idle();if(error) return error;
    for(unsigned i=0;i<COUNT;i++) if(pool.slots[i].made) {
        if(pool.slots[i].gs) xsdk_texture_free(&pool.slots[i].aux);
        xsdk_texture_free(&pool.slots[i].depth);xsdk_texture_free(&pool.slots[i].color);
    }
    if(pool.gs_open) {
        xsdk_texture_free(&pool.snapshot[1]);xsdk_texture_free(&pool.snapshot[0]);
        xsdk_gpu_command_buffer_free(&pool.recording);
        xsdk_shader_free(&pool.stencil);xsdk_shader_free(&pool.gs_restore);
    }
    xsdk_gpu_free(pool.quad);xsdk_shader_free(&pool.pixel);xsdk_shader_free(&pool.vertex);
    memset(&pool,0,sizeof(pool));return 0;
}
