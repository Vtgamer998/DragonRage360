/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "xb_disc.h"
#include "sys/file.h"
extern AdxPlayer gAdxPlayerTbl[];
extern AfsPartition gAfsPartitionTbl[];
extern AdxFcacheParam gAdxFcacheParam;
extern s32 Heap_GetFreeSize(void);
static void runtime_end_files(XbBattleRuntime *r){
 File_CancelRequests();
 for(unsigned i=0;i<ADX_PLAYER_COUNT;i++){
  if(gAdxPlayerTbl[i].adxt)XbAdx_Destroy(gAdxPlayerTbl[i].adxt);
  Heap_Free(gAdxPlayerTbl[i].work);gAdxPlayerTbl[i].work=NULL;gAdxPlayerTbl[i].adxt=NULL;
 }
 for(unsigned i=0;i<FILE_PT_COUNT;i++){Heap_Free(gAfsPartitionTbl[i].info);gAfsPartitionTbl[i].info=NULL;}
 Heap_Free(gAdxFcacheParam.buf);gAdxFcacheParam.buf=NULL;r->files=0;
}
int XbBattleRuntime_BeginStage(XbBattleRuntime *r,void *stage){
 if(!r || active!=r || !r->managers || !r->audio || r->files || r->stage_geometry || !stage || !XbDisc_Get())return -891;
 size_t need=4096;
 if(gAdxFcacheParam.buf)return -891;
 for(unsigned i=0;i<FILE_PT_COUNT;i++){
  if(gAfsPartitionTbl[i].info || gAfsPartitionTbl[i].infoSize<=0 || gAfsPartitionTbl[i].infoSize>0x200000)return -891;
  need+=(size_t)gAfsPartitionTbl[i].infoSize+64;
 }
 for(unsigned i=0;i<ADX_PLAYER_COUNT;i++){
  if(gAdxPlayerTbl[i].adxt || gAdxPlayerTbl[i].work || gAdxPlayerTbl[i].workSize<128 || gAdxPlayerTbl[i].workSize>0x200000)return -891;
  need+=(size_t)gAdxPlayerTbl[i].workSize+64;
 }
 if(need>(size_t)Heap_GetFreeSize())return -851;
 /* Require the ambience stream before the original stage initializer starts
  * it. File_Init brings up the original queue, players and partitions. */
 dr_span stream;if(XbDisc_FindFile("pzs3us2/65116.bin",&stream))return -894;
 File_Init();File_LoadPartitionNw(2);File_WaitPartition(2);r->files=1;
 int e=XbAdx_LastError();if(!e)e=XbBattleStage_BeginGeometry(stage);
 if(e){runtime_end_files(r);return e;}
 r->stage_geometry=stage;return 0;
}
int XbBattleRuntime_EndStage(XbBattleRuntime *r){
 if(!r || active!=r || !r->stage_geometry || !r->files)return -891;
 int e=XbBattleStage_EndGeometry(r->stage_geometry);if(e)return e;
 r->stage_geometry=NULL;runtime_end_files(r);return 0;
}

int XbBattleRuntime_ServiceStage(XbBattleRuntime *r){
 if(!r || active!=r || !r->stage_geometry || !r->files)return -891;
 extern void BtlStage_Update(void);BtlStage_Update();return XbAdx_LastError();
}
