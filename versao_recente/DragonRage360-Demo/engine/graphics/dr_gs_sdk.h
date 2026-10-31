/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DR_GS_SDK_H
#define DR_GS_SDK_H
#include <stdint.h>
enum { DR_GS_SDK_ARGUMENT=-530, DR_GS_SDK_READ=-531, DR_GS_SDK_MEMORY=-532 };
/* Only the game's blocking SyncPath(0,0), CT24, 512x448 profile is supported.
 * Store descriptors occupy the original 0x70 bytes. Destination capacity is
 * the caller's responsibility: w*h*3 bytes. Errors never write its pixels. */
int32_t sceGsSyncPath(int32_t mode,uint16_t timeout);
int32_t sceGsSetDefStoreImage(void *,int16_t,int16_t,int16_t,int16_t,int16_t,int16_t,int16_t);
int32_t sceGsExecStoreImage(void *,void *);
#endif
