/* SPDX-License-Identifier: AGPL-3.0-only */
#include "dr_features.h"
#ifndef DR_FEATURE_XBOX
#include "../net_lobby.h"
extern volatile int gPortUnlockAll,gPortUnlockDone;
#ifdef __cplusplus
extern "C" int Port_NetSession(void);
#else
extern int Port_NetSession(void);
#endif
#endif
int DrFeature_OpenLobby(int code,int port,int battle,int time,int speed,int rollback,int delay){
    if((code!=0 && code!=1) || (code?port!=0:port<1 || port>65535) ||
       battle<0 || battle>4 || time<0 || time>4 || speed<50 || speed>200 ||
       rollback<0 || rollback>8 || delay<0 || delay>9)return DR_FEATURE_INVALID;
#ifdef DR_FEATURE_XBOX
    return DR_FEATURE_UNAVAILABLE;
#else
    return Port_LobbyCreate(code,port,battle,time,speed,rollback,delay);
#endif
}
int DrFeature_JoinLobby(const char *address,int port){
    unsigned n=0;
    if(!address || port<1 || port>65535)return DR_FEATURE_INVALID;
    while(address[n]){if(n>=63 || (unsigned char)address[n]<33 || (unsigned char)address[n]>126)return DR_FEATURE_INVALID;n++;}
    if(!n)return DR_FEATURE_INVALID;
#ifdef DR_FEATURE_XBOX
    return DR_FEATURE_UNAVAILABLE;
#else
    return Port_LobbyJoin(address,port);
#endif
}
int DrFeature_UnlockAll(int confirmed){
    if(confirmed!=1)return DR_FEATURE_CONFIRM;
#ifdef DR_FEATURE_XBOX
    return DR_FEATURE_UNAVAILABLE;
#else
    if(Port_NetSession())return DR_FEATURE_ONLINE;
    if(gPortUnlockAll)return DR_FEATURE_BUSY;
    gPortUnlockDone=0;gPortUnlockAll=1;return 1;
#endif
}
int DrFeature_UnlockStatus(void){
#ifdef DR_FEATURE_XBOX
    return DR_FEATURE_UNAVAILABLE;
#else
    return gPortUnlockAll?1:gPortUnlockDone?2:0;
#endif
}
