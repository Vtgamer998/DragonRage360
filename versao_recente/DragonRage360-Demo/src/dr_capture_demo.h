/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DR_CAPTURE_DEMO_H
#define DR_CAPTURE_DEMO_H
#include <stdint.h>
int dr_capture_demo_open(uint32_t width,uint32_t height);
int dr_capture_demo_draw(void);
int dr_capture_demo_close(void);
#endif
