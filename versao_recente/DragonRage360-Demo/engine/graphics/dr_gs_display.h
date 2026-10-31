/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DR_GS_DISPLAY_H
#define DR_GS_DISPLAY_H
#include <stdint.h>
/* Small engine/backend boundary: no capture types in original game units. */
int dr_gs_backend_display(uint32_t fbp,int x,int y);
void dr_gs_backend_reject(void);
#endif
