/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DR_GS_MOVIE_H
#define DR_GS_MOVIE_H
#include <stddef.h>
#include <stdint.h>
/* Typed native descriptor, not a legacy pointer-as-address DMA chain.
 * Caller owns tags >=128 bytes and image until Send returns. */
int dr_gs_movie_build(void *tags,const uint8_t *image,int step,int x,int y,int w,int h);
int dr_gs_movie_send(const void *tags);
#endif
