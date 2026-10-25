/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef XB_SFX_H
#define XB_SFX_H
#include <stdint.h>
int XbSfx_Bind(void *,int,int);
int XbSfx_Mix(float *planar_6_channels);
int XbSfx_LastError(void);
int XbSfx_ActiveVoices(void);
void XbSfx_Close(void);
int func_002967C0(int,int,...);
int sceSifCallRpc(void *,int,int,void *,int,void *,int,void *,void *);
#endif
