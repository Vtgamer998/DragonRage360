/* SPDX-License-Identifier: AGPL-3.0-or-later
 * API state/lifetime model, no rasterization or hardware execution. */
#define main previous_native_tests
#include "test_xenos.c"
#undef main
#include "dr_targets.h"
int main(void)
{
    previous_native_tests();
    assert(dr_xenos_open()==0);
    uint32_t white;const uint8_t rgba[]={255,255,255,128};
    assert(dr_xenos_texture_create(1,1,rgba,&white)==0);
    assert(dr_xenos_target_create_gs(0,1024,1024)==0);
    assert(dr_xenos_target_create_gs(1,640,480)==0);
    assert(dr_targets_is_gs(0) && dr_targets_is_gs(1) && !dr_targets_is_gs(23));
    assert(dr_targets_texture(0,0) && dr_targets_texture(0,1) && dr_targets_texture(0,2));
    DrGsVertex v[3]={{0,0,.5f,128,128,128,128,0,0,1},
        {32,0,.5f,128,128,128,128,1,0,1},{0,32,.5f,128,128,128,128,0,1,1}};
    DrXenosDraw d={.logical_width=1024,.logical_height=1024,.depth=1,.depth_write=1,
        .scissor={0,0,640,480},.gs_material=1,.textured=1,.tcc=1,.fba=1};
    model_enabled=1;
    assert(dr_xenos_begin_target(0)==0 && tile_recording && edram_color==0);
    d.date=1;unsigned before=draws;
    assert(dr_xenos_draw(v,3,white,&d)==DR_XENOS_STATE && draws==before);d.date=0;
    next_color=0x12345678;next_depth=.25f;next_aux=128;next_stencil=0xa5;
    assert(dr_xenos_draw(v,3,white,&d)==0);
    assert(dr_xenos_snapshot(0)==0 && tile_recording);
    const XsdkTexture *date=dr_targets_snapshot_texture(0,0,1024,1024);
    assert(date && images[date->address/65536].color==128);
    assert(!dr_targets_snapshot_texture(0,1,1024,1024));
    assert(edram_color==0x12345678 && edram_depth==.25f && edram_aux==128 && edram_stencil==0xa5);
    assert(stencil_restores>=9);
    d.date=2;next_color=0x87654321;next_aux=244;next_stencil=0x5a;
    assert(dr_xenos_draw(v,3,white,&d)==0);
    assert(images[date->address/65536].color==128); // frozen, distinct from current aux
    assert(dr_xenos_snapshot(1)==0);
    const XsdkTexture *aux=dr_targets_snapshot_texture(1,0,1024,1024);
    assert(aux && aux!=date && images[aux->address/65536].color==244);
    assert(images[date->address/65536].color==128);
    d.date=0;
    assert(dr_xenos_select_target(1)==0 && edram_color==0 && edram_stencil==0);
    next_color=0x11223344;next_aux=127;next_depth=.75f;next_stencil=255;
    assert(dr_xenos_draw(v,3,white,&d)==0);
    assert(dr_xenos_select_target(0)==0 && edram_color==0x87654321 && edram_stencil==0x5a);
    assert(edram_aux==244 && edram_depth==.25f);
    unsigned passes=tile_passes;
    d.textured=2;d.texture_rect[2]=d.texture_rect[3]=1;
    assert(dr_xenos_draw(v,3,dr_xenos_target_texture(1),&d)==0 && tile_passes>passes);
    assert(bound_textures[0]->address==dr_targets_texture(1,0)->address);
    assert(bound_textures[2]->address==dr_targets_texture(1,1)->address);
    assert(bound_samplers[0]==bound_samplers[2]);
    assert(images[bound_textures[2]->address/65536].color==127);
    assert(dr_xenos_draw(v,3,dr_xenos_target_texture(0),&d)==0); // feedback is resolved before sampled
    assert(bound_textures[2]->address==dr_targets_texture(0,1)->address);
    before=draws;assert(dr_xenos_draw(v,3,white,&d)==DR_XENOS_INVALID && draws==before);
    assert(dr_xenos_end()==0 && !tile_recording);
    unsigned frees=texture_frees;
    wait_result=STATUS_IO_TIMEOUT;
    assert(dr_xenos_target_drop(0)==DR_XENOS_GPU && texture_frees==frees);
    wait_result=STATUS_SUCCESS;
    assert(dr_xenos_target_drop(0)==0 && !dr_targets_snapshot_texture(0,0,1024,1024));
    assert(dr_xenos_target_create_gs(0,64,64)==0);
    assert(dr_xenos_begin_target(0)==0);
    tile_end_result=STATUS_BUFFER_TOO_SMALL;
    assert(dr_xenos_end()==DR_XENOS_GPU && !tile_recording);
    tile_end_result=STATUS_SUCCESS;
    assert(dr_xenos_close()==0);model_enabled=0;
    for(unsigned i=0;i<4096;i++) assert(!images[i].live);
    puts("PASS: GS MRT/tile call model, A/B/A color/raw-alpha/depth/stencil, frozen DATE/effect snapshots, target sampling/feedback, timeout ownership, failed recording cleanup; NO GPU execution");
}
