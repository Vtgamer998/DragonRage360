/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DR_GS_PROFILE_H
#define DR_GS_PROFILE_H
#include <stdint.h>
typedef struct {uint64_t build_us,submit_us;uint32_t source_draws,vertices,replay_commands,merged_draws;} DrGsReplayProfile;
void dr_gs_replay_profile(DrGsReplayProfile *);
#endif
