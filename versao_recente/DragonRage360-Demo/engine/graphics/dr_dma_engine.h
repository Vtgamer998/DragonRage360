/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DR_DMA_ENGINE_H
#define DR_DMA_ENGINE_H
#include <stdint.h>
/* Requires an already opened shared GS session. Does not initialize GPU,
 * window or PS2 globals. One owner/thread; no guessed engine pointers. */
void Dma_InitBuffers(void);
void Dma_SendGif(uint32_t address,uint32_t qwc);
int dr_dma_engine_init(void);
int dr_dma_engine_close(void);
int dr_dma_engine_error(void);
/* The boot producer in gfx.c writes u64 words. Serialize explicitly before
 * passing a bounded virtual address through the original Dma_SendGif ABI. */
int dr_dma_engine_send_u64(const void *words,uint32_t qwc);
#endif
