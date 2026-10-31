/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DR_GS_BACKEND_H
#define DR_GS_BACKEND_H
#include "gs_draw.h"
/* Experimental GsBackend implementation. Unsupported frame state is a hard
 * reported error; the object is not a complete GS renderer. */
extern GsBackend sXenosBackend;
int dr_gs_backend_error(uint32_t *failed_draw);
int dr_gs_backend_close(void);
int dr_gs_backend_sync(void);
/* Lifetime frame_end CPU stage costs in microseconds; single-threaded telemetry. */
void dr_gs_backend_frame_times(uint64_t *movie_us,uint64_t *present_us);
/* Explicit displayed GS framebuffer; never infer it from the last draw.
 * Native fixed-match profile supports original 0/0x70 buffers at zero offset. */
int dr_gs_backend_display(uint32_t fbp,int x,int y);
/* Borrow the application's GPU/present lifecycle; frameEnd draws without presenting. */
int dr_gs_backend_attach(uint32_t width,uint32_t height);
void dr_gs_backend_reject(void);
#endif
