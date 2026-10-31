/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "sys/file.h"
#define HEAP_ANY 2
#include "xb_resconv.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
AfsPartition gAfsPartitionTbl[FILE_PT_COUNT];
AdxPlayer gAdxPlayerTbl[ADX_PLAYER_COUNT];
AdxDvdFsParam gAdxDvdFsParam;
AdxFcacheParam gAdxFcacheParam;
char gFileRootDir[]="data/",gFileDirName[]="cache";
static int allocs,fail_alloc,players,fail_player,mode,polls,requests,live;
static int stat[3];
void *Heap_Alloc(s32 n,s32 alignment,s32 zero,s32 kind){
 (void)alignment;(void)zero;(void)kind;assert(n>0);if(++allocs==fail_alloc)return NULL;
 void *p=calloc(1,n);assert(p);live++;return p;
}
static void release(void *p){if(p){free(p);live--;}}
static void ADXPS2_SetupDvdFs(AdxDvdFsParam *p){assert(p->rootDir==gFileRootDir);}
static void ADXPS2_LoadFcacheDvd(AdxFcacheParam *p){assert(p->buf && p->bufSize==0x190);}
static void ADXPS2_SetupThrd(void *p,s32 n){assert(!p && !n);}
static void ADXT_Init(void){}
void Vsync_InstallHandler(void){}
static void File_AdxErrorCallback(void *p,char *s){(void)p;(void)s;}
static void ADXERR_EntryErrFunc(void (*p)(void *,char *),void *ctx){assert(p==File_AdxErrorCallback && !ctx);}
static ADXT_HN ADXT_Create(s32 n,void *p,s32 size){assert(n==2 && p && size>=128);return ++players==fail_player?NULL:p;}
static void ADXT_SetReloadSct(ADXT_HN p,s32 n){assert(p && n==16);}
static void ADXT_SetSvrFreq(ADXT_HN p,s32 n){assert(p && n==30);}
static s32 ADXF_LoadPartitionNw(s32 id,char *name,void *p,void *info){assert(id>=0 && id<2 && name && !p && info);stat[id]=(mode==1 && id==0)?ADXF_STAT_ERROR:(mode==2 && id==1)?0:ADXF_STAT_READEND;return 0;}
static s32 ADXF_GetPtStat(s32 pt){return stat[pt];}
static void ADXM_ExecMain(void){assert(++polls<=2400);}
void File_InitRequests(void){requests++;}
/* ORIGINAL_FUNCTIONS */
static void clear(void){
 for(unsigned i=0;i<ADX_PLAYER_COUNT;i++){release(gAdxPlayerTbl[i].work);gAdxPlayerTbl[i].work=NULL;gAdxPlayerTbl[i].adxt=NULL;}
 for(unsigned i=0;i<FILE_PT_COUNT;i++){release(gAfsPartitionTbl[i].info);gAfsPartitionTbl[i].info=NULL;}
 release(gAdxFcacheParam.buf);gAdxFcacheParam.buf=NULL;assert(!live);
}
static void run(int allocation,int player_failure,int io){
 allocs=players=polls=requests=0;fail_alloc=allocation;fail_player=player_failure;mode=io;
 assert(!XbRes_BeginCheckedReads());File_Init();
 if(allocation || player_failure || io)assert(XbRes_LastError()<0);
 else {assert(!XbRes_LastError() && players==6 && requests==1);for(unsigned i=0;i<6;i++)assert(gAdxPlayerTbl[i].adxt);}
 clear();XbRes_EndCheckedReads();
}
int main(void){
 for(unsigned i=0;i<3;i++){gAfsPartitionTbl[i].name="partition";gAfsPartitionTbl[i].infoSize=128;}
 for(unsigned i=0;i<6;i++){gAdxPlayerTbl[i].workSize=128;gAdxPlayerTbl[i].reloadSct=16;}
 for(int i=1;i<=10;i++)run(i,0,0);
 for(int i=1;i<=6;i++)run(0,i,0);
 run(0,0,1);run(0,0,2);run(0,0,0);
 puts("PASS original File_Init/ADX ownership: 10 allocation failures, 6 player failures, partition error and bounded timeout, then six-player success; mock devices, no audio");
}
