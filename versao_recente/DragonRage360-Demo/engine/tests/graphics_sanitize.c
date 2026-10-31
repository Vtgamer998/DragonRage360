/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dr_gs_texture.h"
static uint32_t seed=360;
static uint32_t next(void) {seed=seed*1664525+1013904223;return seed;}
int main(void)
{
    unsigned char *vram=calloc(1,0x400000);assert(vram);
    unsigned char out[4096];assert(dr_gs_texture_selftest());
    /* Known RGBA32 pixel: canonical bytes must stay RGBA on every CPU. */
    vram[0]=0x11;vram[1]=0x22;vram[2]=0x33;vram[3]=0x80;
    DrGsTexture t={.width=1,.height=1,.tbw=1,.psm=0};
    assert(dr_gs_texture_decode(vram,0x400000,&t,out,sizeof(out))==0);
    assert(!memcmp(out,"\x11\x22\x33\x80",4));
    t.psm=1;t.ta0=64;
    assert(!dr_gs_texture_decode(vram,0x400000,&t,out,sizeof(out)) && out[3]==64);
    memset(vram,0,4);t.aem=1;
    assert(!dr_gs_texture_decode(vram,0x400000,&t,out,sizeof(out)) && out[3]==0);
    t.psm=2;t.ta1=128;vram[0]=vram[1]=255;
    assert(!dr_gs_texture_decode(vram,0x400000,&t,out,sizeof(out)));
    assert(out[0]==248 && out[1]==248 && out[2]==248 && out[3]==128);
    const unsigned formats[]={0,1,2,0xa,0x13,0x14,0x1b,0x24,0x2c,0x3f};
    for(unsigned i=0;i<10000;i++) {
        t=(DrGsTexture){.width=next()%40,.height=next()%40,.tbp=next()%20000,.tbw=next()%80,
            .psm=formats[next()%10],.cbp=next()%20000,.cpsm=next()%16,.csm=next()%3,.ta0=next()%256,.ta1=128,.aem=next()%2};
        memset(out,'Q',sizeof(out));int error=dr_gs_texture_decode(vram,0x400000,&t,out,sizeof(out));
        if(error) for(unsigned j=0;j<sizeof(out);j++) assert(out[j]=='Q');
    }
    free(vram);puts("PASS: texture golden RGBA byte order + 10000 descriptors under ASan/UBSan");
}
