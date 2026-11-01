/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "gfx_probe.h"
#include "xsdk_compat.h"
#include "dr_xenos.h"
static const DrGsVertex triangle[3]={
    {0.0f,0.5f,0.5f,255,0,0,255,0.0f,0.0f,1.0f},
    {0.5f,-0.5f,0.5f,0,255,0,255,1.0f,0.0f,1.0f},
    {-0.5f,-0.5f,0.5f,0,0,255,255,0.0f,1.0f,1.0f}};
int D50_GfxProbeRun(uint32_t width,uint32_t height)
{
    if(width<16 || width>1280 || height<16 || height>720 || (width&31)){
        D50_Log("D50 probe: invalid size %ux%u",width,height);
        return DR_XENOS_INVALID;
    }
    uint64_t started=D50_NowUs();
    int error=dr_xenos_open();
    uint32_t texture=0;
    if(!error){
        const unsigned char white[4]={255,255,255,255};
        error=dr_xenos_texture_create(1,1,white,&texture);
    }
    if(!error)error=dr_xenos_begin(width,height);
    if(!error){
        DrXenosDraw draw={0};
        draw.logical_width=width;draw.logical_height=height;
        draw.topology=0;draw.blend=0;draw.depth=1;draw.depth_write=1;
        draw.scissor[0]=0;draw.scissor[1]=0;draw.scissor[2]=(int)width;draw.scissor[3]=(int)height;
        error=dr_xenos_draw(triangle,3,texture,&draw);
    }
    if(!error)error=dr_xenos_end();
    if(!error)error=dr_xenos_sync();
    if(texture)dr_xenos_texture_destroy(texture);
    int closed=dr_xenos_close();
    if(!error)error=closed;
    uint64_t elapsed=D50_NowUs()-started;
    if(error)D50_Log("FAIL: D50 gfx probe size=%ux%u error=%d elapsed_us=%llu",width,height,error,(unsigned long long)elapsed);
    else D50_Log("PASS: D50 gfx probe clear+triangle size=%ux%u elapsed_us=%llu",width,height,(unsigned long long)elapsed);
    return error;
}
