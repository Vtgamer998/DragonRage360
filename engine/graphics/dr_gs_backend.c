/* SPDX-License-Identifier: AGPL-3.0-or-later
 * GsBackend ABI: upstream 3ca419c, gs_draw.h. GPU contracts: public xsdk
 * 57b02f1e, include/xsdk/{gpu,present,texture}.h. No hardware result claimed.
 */
#include "dr_gs_backend.h"
#include "dr_gs_replay.h"
#include "dr_targets.h"
#include "xsdk/gpu.h"
#include "xsdk/present.h"
#include "xsdk/log.h"
#include <stdlib.h>
#include <string.h>
enum { TEXTURES=256,PIPES=1024 };
static int attached;
static uint32_t attached_width,attached_height;
static struct {
    int opened,error;uint32_t failed,width,height,white_native,serial,pipe_count;
    GsTex white,movie[2];
    int display_set;uint32_t display_fbp;
    uint32_t keys[PIPES];
    struct {GsTex handle;uint32_t width,height,levels;} texture[TEXTURES];
} state;
static void fail(int error,uint32_t draw)
{
    if(!state.error) {state.error=error;state.failed=draw;xsdk_log_write("GS Xenos FAIL %d, draw %u",error,(unsigned)draw);}
}
void dr_gs_backend_reject(void) {fail(DR_GS_UNSUPPORTED,UINT32_MAX);}
int dr_gs_backend_error(uint32_t *failed)
{if(failed) *failed=state.failed;return state.error;}
int dr_gs_backend_display(uint32_t fbp,int x,int y)
{
    if(!state.opened)return DR_XENOS_STATE;
    if(state.error)return state.error;
    if((fbp!=0 && fbp!=0x70) || x || y)return DR_GS_UNSUPPORTED;
    state.display_fbp=fbp;state.display_set=1;return 0;
}
static uint32_t native_texture(GsTex handle,void *unused)
{
    (void)unused;
    if(handle&0x80000000u) {
        unsigned id=((uint32_t)handle&0x7fffffffu)>>2,attachment=(uint32_t)handle&3;
        return attachment==1?dr_xenos_target_texture(id):0;
    }
    unsigned slot=(unsigned)(handle&511);
    if(!slot || slot>TEXTURES || state.texture[slot-1].handle!=handle) return 0;
    return slot;
}
static GsTex tex_create(uint32_t w,uint32_t h,int format,int levels)
{
    if(state.error || format!=GS_TEXFMT_RGBA8 || levels<1 || state.serial==0x3fffff) return 0;
    uint32_t native;int e=dr_xenos_texture_allocate(w,h,(uint32_t)levels,&native);
    if(e) {xsdk_log_write("GS texture allocation error=%d width=%u height=%u levels=%d slots=%u",e,w,h,levels,dr_xenos_texture_slots());fail(e,UINT32_MAX);return 0;}
    GsTex handle=(GsTex)(++state.serial<<9)|native;
    state.texture[native-1].handle=handle;state.texture[native-1].width=w;
    state.texture[native-1].height=h;state.texture[native-1].levels=(uint32_t)levels;
    return handle;
}
static int tex_upload(GsTex handle,int level,uint32_t w,uint32_t h,int format,const void *pixels,uint32_t bytes)
{
    uint32_t native=native_texture(handle,NULL);int e=DR_XENOS_INVALID;
    if(native && !(handle&0x80000000u) && level>=0 && (uint32_t)level<state.texture[native-1].levels && format==GS_TEXFMT_RGBA8) {
        uint32_t ew=state.texture[native-1].width>>level,eh=state.texture[native-1].height>>level;
        if(w==(ew?ew:1) && h==(eh?eh:1)) e=dr_xenos_texture_upload(native,(uint32_t)level,pixels,bytes);
    }
    free((void *)pixels);
    if(e) fail(e,UINT32_MAX);
    return e==0;
}
static void tex_destroy(GsTex handle)
{
    if(!handle) return;
    uint32_t native=native_texture(handle,NULL);
    if(!native || handle==state.white || (handle&0x80000000u)) {fail(DR_XENOS_INVALID,UINT32_MAX);return;}
    int e=dr_xenos_texture_destroy(native);
    if(e) fail(e,UINT32_MAX);else memset(&state.texture[native-1],0,sizeof(state.texture[0]));
}
static GsTex white_tex(void) {return state.white;}
static int tex_slots(void) {return state.error?0:(int)dr_xenos_texture_slots();}
static int pipe_get(uint32_t key)
{
    for(uint32_t i=0;i<state.pipe_count;i++) if(state.keys[i]==key) return (int)i;
    if(state.pipe_count==PIPES) {xsdk_log_write("GS pipeline table full key=%08x",key);fail(DR_XENOS_FULL,UINT32_MAX);return -1;}
    state.keys[state.pipe_count]=key;return (int)state.pipe_count++;
}
static void target_ensure(int id)
{
    if(state.error) return;
    if(id<0 || id>=MAX_TARGETS || gsScale4!=4) {fail(DR_GS_UNSUPPORTED,UINT32_MAX);return;}
    int e=dr_xenos_target_create_gs((unsigned)id,GS_W,GS_H);if(e) fail(e,UINT32_MAX);
}
static void target_drop(int id)
{
    if(id<0 || id>=MAX_TARGETS) {fail(DR_XENOS_INVALID,UINT32_MAX);return;}
    if(!dr_targets_texture((unsigned)id,0)) return;
    int e=dr_xenos_target_drop((unsigned)id);if(e) fail(e,UINT32_MAX);
}
static GsTex target_handle(int id,unsigned attachment)
{return id>=0 && id<MAX_TARGETS && dr_targets_texture((unsigned)id,attachment)?(GsTex)(0x80000000u|((unsigned)id<<2)|(attachment+1)):0;}
static GsTex target_color(int id) {return target_handle(id,0);}
static GsTex target_aux(int id) {return target_handle(id,1);}
static GsTex target_depth(int id) {return target_handle(id,2);}
static void scale_changed(void)
{
    if(gsScale4!=4) {fail(DR_GS_UNSUPPORTED,UINT32_MAX);return;}
    for(int i=0;i<MAX_TARGETS;i++) target_drop(i);
    gsTargetW=GS_W;gsTargetH=GS_H;
}
static int init(void)
{
    if(state.opened) return 0;
    memset(&state,0,sizeof(state));state.failed=UINT32_MAX;
    if (!attached) {
        NTSTATUS s=xsdk_gpu_open();if(!NT_SUCCESS(s)) {fail(DR_XENOS_GPU,UINT32_MAX);return 0;}
        s=xsdk_present_open(XSDK_DISPLAY_720P,2,1,&state.width,&state.height);
        if(!NT_SUCCESS(s)) {fail(DR_XENOS_GPU,UINT32_MAX);(void)xsdk_gpu_close();return 0;}
    } else {state.width=attached_width;state.height=attached_height;}
    int e=dr_xenos_open();
    if(e) {fail(e,UINT32_MAX);if(!attached) {xsdk_present_close();(void)xsdk_gpu_close();}return 0;}
    state.opened=1;gsScale4=4;gsTargetW=GS_W;gsTargetH=GS_H;
    state.white=tex_create(1,1,GS_TEXFMT_RGBA8,1);
    uint8_t *pixel=malloc(4);
    if(!pixel) {fail(DR_GS_MEMORY,UINT32_MAX);(void)dr_gs_backend_close();return 0;}
    memcpy(pixel,"\xff\xff\xff\x80",4);
    if(!state.white || !tex_upload(state.white,0,1,1,GS_TEXFMT_RGBA8,pixel,4)) {
        if(!state.white) free(pixel);
        (void)dr_gs_backend_close();return 0;
    }
    state.white_native=native_texture(state.white,NULL);return 1;
}
int dr_gs_backend_attach(uint32_t width,uint32_t height)
{
    if(state.opened || !width || !height || width>1280 || height>720 || (width&31)) return DR_XENOS_INVALID;
    attached=1;attached_width=width;attached_height=height;return 0;
}
int dr_gs_backend_close(void)
{
    if(!state.opened) return 0;
    int e=dr_xenos_close();if(e) {fail(e,UINT32_MAX);return e;}
    if(!attached) {
        xsdk_present_close();NTSTATUS s=xsdk_gpu_close();
        if(!NT_SUCCESS(s)) {fail(DR_XENOS_GPU,UINT32_MAX);return DR_XENOS_GPU;}
    }
    attached=0;
    state.opened=0;return 0;
}
int dr_gs_backend_sync(void)
{
    if(state.error) return state.error;
    if(!state.opened) return DR_XENOS_INVALID;
    int e=dr_xenos_sync();if(e) fail(e,UINT32_MAX);return e;
}
static int target_read(int id,uint8_t *rgba)
{
    if(!rgba || id<0 || !dr_targets_ready((unsigned)id)) return 0;
    int e=dr_xenos_sync();if(e) {fail(e,UINT32_MAX);return 0;}
    uint8_t *row=malloc(GS_W*4);if(!row) {fail(DR_GS_MEMORY,UINT32_MAX);return 0;}
    for(unsigned y=0;y<448;y++) {
        xsdk_texture_download_rows(dr_targets_texture((unsigned)id,0),0,y,1,row,GS_W*4);
        memcpy(rgba+y*512*4,row,512*4);
    }
    free(row);return 1;
}
static int blit(int target,uint32_t width,uint32_t height,int copy)
{
    DrGsVertex quad[6]={{0,0,0,128,128,128,128,0,0,1},{(float)width,0,0,128,128,128,128,1,0,1},
        {0,(float)height,0,128,128,128,128,0,1,1},{0,(float)height,0,128,128,128,128,0,1,1},
        {(float)width,0,0,128,128,128,128,1,0,1},{(float)width,(float)height,0,128,128,128,128,1,1,1}};
    DrXenosDraw d={.logical_width=width,.logical_height=height,.depth=1,.gs_material=1,
        .textured=2,.tcc=1,.texture_rect={0,0,1,1},.scissor={0,0,(int)width,(int)height}};
    if(!copy) for(unsigned i=0;i<6;i++) {quad[i].s*=.5f;quad[i].t*=448.0f/1024;}
    return dr_xenos_draw(quad,6,dr_xenos_target_texture((unsigned)target),&d);
}
static void target_copy(int src,int dst)
{
    if(src<0 || dst<0 || !dr_targets_ready((unsigned)src) || !dr_targets_is_gs((unsigned)dst)) {fail(DR_XENOS_INVALID,UINT32_MAX);return;}
    if(src==dst) return;
    int e=dr_xenos_begin_target((unsigned)dst);if(e) {fail(e,UINT32_MAX);return;}
    e=blit(src,GS_W,GS_H,1);int end=dr_xenos_end();if(!e)e=end;if(e)fail(e,UINT32_MAX);
}
/* Snapshot after the final IMAGE payload reached VRAM, before replay. The
 * two private textures are reused, so one movie frame does not leak slots. */
static uint32_t movie_texture(unsigned second) {
    extern uint32_t Gs_VramRead(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);
    uint8_t *rgba=malloc(512*448*4);if(!rgba){fail(DR_GS_MEMORY,UINT32_MAX);return 0;}
    for(unsigned y=0;y<448;y++)for(unsigned x=0;x<512;x++) {
        uint32_t pixel=Gs_VramRead(second?0xe00:0,8,0,x,y);
        for(unsigned b=0;b<4;b++)rgba[(y*512+x)*4+b]=(uint8_t)(pixel>>(b*8));
    }
    if(!state.movie[second])state.movie[second]=tex_create(512,448,GS_TEXFMT_RGBA8,1);
    if(!state.movie[second]){free(rgba);return 0;}
    if(!tex_upload(state.movie[second],0,512,448,GS_TEXFMT_RGBA8,rgba,512*448*4))return 0;
    return native_texture(state.movie[second],NULL);
}
static void frame_end(void)
{
    if(state.error || !state.opened) return;
    uint32_t movies[2]={0};
    for(unsigned i=0;i<gsDrawCount;i++)if(gsDraws[i].native==100 || gsDraws[i].native==101) {
        unsigned second=(unsigned)gsDraws[i].native-100;
        if(gsDraws[i].target<0 || gsDraws[i].target>=gsTargetCount ||
           gsTargets[gsDraws[i].target].fbp!=(second?0x70u:0u)){fail(DR_GS_UNSUPPORTED,i);return;}
        if(!movies[second])movies[second]=movie_texture(second);
        if(!movies[second])return;
        if(!state.pipe_count && pipe_get(1u<<10)<0)return;
    }
    DrGsReplay replay={.movie_texture={movies[0],movies[1]},.vu_vertices=gsVuVerts,.vu_indices=gsVuIdx,.vu_uniforms=gsVuUni,
        .vu_vertex_count=gsVuVertCount,.vu_index_count=gsVuIdxCount,.vu_uniform_count=gsVuUniCount,.depth_max=gsZMax,
        .logical_width=GS_W,.logical_height=GS_H,.saved_targets=1,
        .white_texture=state.white_native,.pipeline_keys=state.keys,.pipeline_count=state.pipe_count,
        .texture=native_texture};
    uint32_t failed=UINT32_MAX;int e=0;
    if(gsDrawCount) e=dr_gs_replay(&replay,gsDraws,gsDrawCount,gsVerts,gsVertCount,&failed);
    if(e) {
        if(failed<gsDrawCount){const GsDraw *d=&gsDraws[failed];
            xsdk_log_write("D36 rejected state: key=%08x target=%d vu=%d native=%d wall=%d sampler=%d mode=%d,%d,%d,%d misc=%.2f,%.2f,%.2f,%.2f target_tex=%d",d->pipeline>=0 && (unsigned)d->pipeline<state.pipe_count?state.keys[d->pipeline]:0,d->target,d->vu,d->native,d->wall,d->sampler,d->mode[0],d->mode[1],d->mode[2],d->mode[3],(double)d->misc[0],(double)d->misc[1],(double)d->misc[2],(double)d->misc[3],d->tex_is_target);
        }
        fail(e,failed);return;
    }
    for(int i=0;i<gsTargetCount;i++) if(dr_targets_ready((unsigned)i)) gsTargets[i].cleared=1;
    int best=-1;
    for(int i=0;i<gsTargetCount;i++) if(dr_targets_ready((unsigned)i)) {
        if(state.display_set) {
            if(gsTargets[i].fbp==state.display_fbp){best=i;break;}
        } else if(best<0 || (int)gsTargets[i].fbp==gGsMainFbp)best=i;
    }
    if(state.display_set && best<0){fail(DR_XENOS_STATE,UINT32_MAX);return;}
    if(best>=0) {
        uint32_t address;
        if(!attached && !NT_SUCCESS(xsdk_present_begin(&address,1000))) {fail(DR_XENOS_GPU,UINT32_MAX);return;}
        e=dr_xenos_begin(state.width,state.height);
        if(!e) {e=blit(best,state.width,state.height,0);int end=dr_xenos_end();if(!e)e=end;}
        if(e) {fail(e,UINT32_MAX);return;}
        if(!attached) {xsdk_gpu_resolve(address,state.width,state.height,0);xsdk_present_end();}
    }
    for(int i=0;i<gsTargetCount;i++) gsTargets[i].draws=0;
    gsDrawCount=gsVertCount=gsVuVertCount=gsVuIdxCount=gsVuUniCount=0;gsAnchor=0;gsSkipped=gsNative=0;
    extern unsigned gGsFbUploads;gGsFbUploads=0;
}
GsBackend sXenosBackend={.name="xenos",.init=init,.frameEnd=frame_end,.scaleChanged=scale_changed,
    .whiteTex=white_tex,.texCreate=tex_create,.texUpload=tex_upload,.texDestroy=tex_destroy,.texSlotsLeft=tex_slots,
    .targetEnsure=target_ensure,.targetDrop=target_drop,.targetColor=target_color,.targetAux=target_aux,
    .targetDepth=target_depth,.pipeGet=pipe_get,.targetRead=target_read,.targetCopy=target_copy};
