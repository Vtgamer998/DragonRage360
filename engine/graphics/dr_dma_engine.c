/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "dr_dma_engine.h"
#include "dr_dma_producer.h"
#include "dr_gs_native.h"
#include <stdlib.h>
#include <string.h>
#ifdef DR_XBOX360
#include <xsdk/log.h>
#endif
static int error,owned;
static DrGsSpan gif;
static int fail(int e) {if(e && !error)error=e;return error;}
int dr_dma_engine_error(void) {return error?error:dr_dma_producer_error();}
static int submit(void *opaque,const DrGsSpan *spans,size_t n,uint32_t address) {
    (void)opaque;int e=dr_gs_native_bind(spans,n);
    if(e)return e;
    e=dr_gs_native_vif1(address,1);
#ifdef DR_XBOX360
    if(e){DrGsStreamStats s={0};dr_gs_native_stats(&s);xsdk_log_write("VIF submit error=%d address=%08x command=%08x tags=%u words=%u calls=%u",e,s.fail_address,s.fail_command,s.tags,s.words,s.vu_calls);for(size_t i=0;i<n;i++)if(s.fail_address>=spans[i].address){size_t at=s.fail_address-spans[i].address;if(at<=spans[i].size && 16<=spans[i].size-at){const unsigned char *p=(const unsigned char*)spans[i].bytes+at;uint32_t w[4];for(unsigned k=0;k<4;k++)w[k]=p[k*4]|(uint32_t)p[k*4+1]<<8|(uint32_t)p[k*4+2]<<16|(uint32_t)p[k*4+3]<<24;xsdk_log_write("VIF failing DMA tag %08x %08x %08x %08x",w[0],w[1],w[2],w[3]);break;}}}
#endif
    dr_gs_native_unbind();return e;
}
int dr_dma_engine_init(void) {
    if(error)return error;
    int e=dr_gs_native_error();if(e)return fail(e);
    if(dr_dma_producer_is_open())return fail(owned?dr_dma_producer_error():DR_DMA_STATE);
    e=dr_dma_producer_open_mapped(submit,NULL);if(!e)owned=1;return fail(e);
}
void Dma_InitBuffers(void) {(void)dr_dma_engine_init();}
int dr_dma_engine_close(void) {
    if(gif.bytes)return DR_DMA_STATE;
    int e=owned?dr_dma_producer_close():0;
    if(!e) {owned=0;error=0;}return e;
}
void Dma_SendGif(uint32_t address,uint32_t qwc) {
    if(error)return;
    /* The sole original caller is Gfx_Init's u64 array. It is adapted to the
     * typed wrapper below; arbitrary address casts never become native reads. */
    if(!gif.bytes || address!=gif.address || qwc<1 || (uint64_t)qwc*16!=gif.size) {
        fail(DR_DMA_INVALID);return;
    }
    int e=dr_gs_native_bind(&gif,1);if(e){fail(e);return;}
    e=dr_gs_native_gif(address,qwc,0);dr_gs_native_unbind();fail(e);
}
int dr_dma_engine_send_u64(const void *words,uint32_t qwc) {
    if(error)return error;
    if(!words || !qwc || qwc>65536 || gif.bytes)return fail(DR_DMA_INVALID);
    int e=dr_gs_native_error();if(e)return fail(e);
    size_t size=(size_t)qwc*16;uint8_t *bytes=malloc(size);
    if(!bytes)return fail(DR_DMA_MEMORY);
    for(size_t i=0;i<size;i+=8) {
        uint64_t word;memcpy(&word,(const uint8_t *)words+i,8);
        for(unsigned j=0;j<8;j++)bytes[i+j]=(uint8_t)(word>>(j*8));
    }
    gif=(DrGsSpan){0x45000000,bytes,size};Dma_SendGif(gif.address,qwc);
    memset(&gif,0,sizeof(gif));free(bytes);return error;
}
