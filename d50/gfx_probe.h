/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef D50_GFX_PROBE_H
#define D50_GFX_PROBE_H
#include <stdint.h>
/* D50 milestone 2: clear plus one triangle through dr_xenos, no game data.
 * Returns a dr_xenos status code (0 = DR_XENOS_OK). The caller must have
 * opened nothing else: the probe owns dr_xenos_open/close. */
int D50_GfxProbeRun(uint32_t width,uint32_t height);
#endif
