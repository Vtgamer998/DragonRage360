/* SPDX-License-Identifier: AGPL-3.0-only */
#include "dr_features.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#ifndef DR_FEATURE_XBOX
volatile int gPortUnlockAll,gPortUnlockDone;
static int online,create_calls,join_calls,port_seen;
#ifdef __cplusplus
extern "C" {
#endif
int Port_NetSession(void){return online;}
int Port_LobbyCreate(int c,int p,int b,int t,int s,int r,int d){assert(c==0 && b==0 && t==0 && s==100 && r==4 && d==0);create_calls++;port_seen=p;return 1;}
int Port_LobbyJoin(const char *a,int p){assert(!strcmp(a,"127.0.0.1"));join_calls++;port_seen=p;return 1;}
#ifdef __cplusplus
}
#endif
#endif
int main(void){
 assert(DrFeature_OpenLobby(2,0,0,0,100,4,0)==DR_FEATURE_INVALID);
 assert(DrFeature_OpenLobby(0,0,0,0,100,4,0)==DR_FEATURE_INVALID);
 assert(DrFeature_OpenLobby(1,28000,0,0,100,4,0)==DR_FEATURE_INVALID);
 assert(DrFeature_OpenLobby(0,28000,5,0,100,4,0)==DR_FEATURE_INVALID);
 assert(DrFeature_OpenLobby(0,28000,0,0,100,9,0)==DR_FEATURE_INVALID);
 assert(DrFeature_JoinLobby(NULL,28000)==DR_FEATURE_INVALID);
 assert(DrFeature_JoinLobby("",28000)==DR_FEATURE_INVALID);
 assert(DrFeature_JoinLobby("127.0.0.1\n",28000)==DR_FEATURE_INVALID);
 assert(DrFeature_JoinLobby("127.0.0.1",65536)==DR_FEATURE_INVALID);
 assert(DrFeature_UnlockAll(0)==DR_FEATURE_CONFIRM);
#ifdef DR_FEATURE_XBOX
 assert(DrFeature_OpenLobby(0,28000,0,0,100,4,0)==DR_FEATURE_UNAVAILABLE);
 assert(DrFeature_JoinLobby("127.0.0.1",28000)==DR_FEATURE_UNAVAILABLE);
 assert(DrFeature_UnlockAll(1)==DR_FEATURE_UNAVAILABLE);
 assert(DrFeature_UnlockStatus()==DR_FEATURE_UNAVAILABLE);
 puts("PASS Xbox: original netcode unavailable, no fake online/cheat success");
#else
 assert(DrFeature_OpenLobby(0,28000,0,0,100,4,0)==1 && create_calls==1 && port_seen==28000);
 assert(DrFeature_JoinLobby("127.0.0.1",28000)==1 && join_calls==1);
 assert(DrFeature_UnlockStatus()==0);online=1;
 assert(DrFeature_UnlockAll(1)==DR_FEATURE_ONLINE && !gPortUnlockAll);online=0;
 assert(DrFeature_UnlockAll(1)==1 && gPortUnlockAll && DrFeature_UnlockStatus()==1);
 assert(DrFeature_UnlockAll(1)==DR_FEATURE_BUSY);gPortUnlockAll=0;gPortUnlockDone=1;
 assert(DrFeature_UnlockStatus()==2);
 puts("PASS upstream forwarding: original lobby calls, original unlock queue, validation, confirmation, online/pending gates");
#endif
 return 0;
}
