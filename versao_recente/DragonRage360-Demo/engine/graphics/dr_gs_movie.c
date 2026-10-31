/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "dr_gs_movie.h"
#include "dr_gs_native.h"
#include <stdlib.h>
#include <string.h>
typedef struct {uint32_t magic,version;const uint8_t *image;int32_t step,x,y,w,h;} Upload;
static int valid(const Upload *s) {
    return s->magic==0x584d4f56 && s->version==1 && s->image &&
        (s->step==0 || s->step==1024) && s->x==0 && (s->y==0 || s->y==448) && s->w==512 && s->h==448;
}
int dr_gs_movie_build(void *tags,const uint8_t *image,int step,int x,int y,int w,int h) {
    if(!tags)return DR_STREAM_INVALID;
    Upload s={0x584d4f56,1,image,step,x,y,w,h};memset(tags,0,128);
    if(!valid(&s))return DR_STREAM_INVALID;
    memcpy(tags,&s,sizeof(s));return 0;
}
static void le64(uint8_t *p,uint64_t v){for(unsigned i=0;i<8;i++)p[i]=(uint8_t)(v>>(i*8));}
int dr_gs_movie_send(const void *tags) {
    if(!tags)return DR_STREAM_INVALID;
    Upload s;memcpy(&s,tags,sizeof(s));if(!valid(&s))return DR_STREAM_INVALID;
    /* One canonical GIF run per column. 28 whole 16x16 macroblocks remain
     * separate IMAGE transfers, preserving the game's column-major layout. */
    const size_t block=6*16+1024,bytes=block*28;
    uint8_t *packet=malloc(bytes);if(!packet)return DR_STREAM_BUDGET;
    int e=0;
    for(unsigned x=0;x<32 && !e;x++) {
        for(unsigned y=0;y<28;y++) {
            uint8_t *p=packet+y*block;
            le64(p,4ull|(1ull<<60));le64(p+8,14);
            le64(p+16,8ull<<48);le64(p+24,0x50);
            /* TRXREG and TRXPOS; TRXDIR follows in its own A+D tag below. */
            le64(p+32,16ull|(16ull<<32));le64(p+40,0x52);
            le64(p+48,(uint64_t)(x*16)<<32|(uint64_t)(s.y+y*16)<<48);le64(p+56,0x51);
            le64(p+64,0);le64(p+72,0x53);
            le64(p+80,64ull|(1ull<<15)|(2ull<<58));le64(p+88,0);
            memcpy(p+96,s.image+(x*28+y)*(size_t)s.step,1024);
        }
        DrGsSpan span={0x46000000,packet,bytes};
        e=dr_gs_native_bind(&span,1);
        if(!e){e=dr_gs_native_gif(span.address,(uint32_t)(bytes/16),0);dr_gs_native_unbind();}
    }
    free(packet);return e;
}
