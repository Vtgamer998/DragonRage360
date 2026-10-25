/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef XB_BATTLE_RUNTIME_H
#define XB_BATTLE_RUNTIME_H
#include <stddef.h>
typedef struct XbBattleRuntime XbBattleRuntime;
/* Dedicated native common resources for object/roster boot. Common 3 and the
 * effect scene are NOT supplied by this interface. No readiness marker is set. */
int XbBattleRuntime_Open(const void *,size_t,const void *,size_t,XbBattleRuntime **);
int XbBattleRuntime_Close(XbBattleRuntime *);
int XbBattleRuntime_HasResources(void);
int XbBattleRuntime_BindLight(void **);
/* After original object init, models 0/1, and grounded starts: initializes a
 * temporary original save/progress, versus setup, original BtlChar and BtlAi.
 * Human side 0 / CPU side 1, character ids 0/1. Does not call Battle_Init or
 * a frame of the effect/camera/HUD scene. End before freeing models. */
int XbBattleRuntime_BeginManagers(XbBattleRuntime *);
int XbBattleRuntime_EndManagers(XbBattleRuntime *);
typedef struct XbBattleRuntimeInfo {int health[2],ki[2],ai_type[2],ai_level[2];} XbBattleRuntimeInfo;
int XbBattleRuntime_GetInfo(XbBattleRuntime *,XbBattleRuntimeInfo *);
#endif
