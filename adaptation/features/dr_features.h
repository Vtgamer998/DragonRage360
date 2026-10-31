/* SPDX-License-Identifier: AGPL-3.0-only */
#ifndef DR_FEATURES_H
#define DR_FEATURES_H
#ifdef __cplusplus
extern "C" {
#endif
enum { DR_FEATURE_INVALID=-1, DR_FEATURE_BUSY=-2, DR_FEATURE_ONLINE=-3,
       DR_FEATURE_CONFIRM=-4, DR_FEATURE_UNAVAILABLE=-100 };
/* These forward to the original upstream lobby, never the old DRL1 lobby.
 * The Xbox backend is deliberately unavailable until its full netcode is ported. */
int DrFeature_OpenLobby(int by_code,int port,int battle,int time,int speed,int rollback,int delay);
int DrFeature_JoinLobby(const char *address,int port);
/* Confirmation belongs to the upstream modal. Unlocking empties records and
 * becomes permanent on the next save; the original game thread performs it. */
int DrFeature_UnlockAll(int confirmed);
int DrFeature_UnlockStatus(void); /* 0 idle, 1 queued, 2 completed */
#ifdef __cplusplus
}
#endif
#endif
