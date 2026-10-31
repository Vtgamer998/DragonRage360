/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "sys/common.h"
#include "sys/heap.h"
#include "xb_resconv.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
CommonRes *gCommonRes;
static int allocs,fail_alloc,live,requests,fail_request,pending,aborts,updates,fail_update,sounds,fail_sound;
static int size_failure,boot_failure,io_busy;
void *Heap_Alloc(s32 n,u32 align,s32 flags,s32 heap){
 (void)align;(void)flags;(void)heap;assert(n>0);allocs++;if(allocs==fail_alloc)return NULL;
 void *p=calloc(1,(size_t)n);assert(p);live++;return p;
}
void Heap_Free(void *p){assert(p && live>0);live--;free(p);}
static void *File_LoadSync(s32 id,void *p,s32 cap){
 assert(id==5 && !p && !cap);if(boot_failure){XbRes_SetReadError(-931);return NULL;}
 return Heap_Alloc(2048,64,0,0);
}
static s32 File_GetSize(s32 id){assert(id>=2 && id<=4);if(id==size_failure){XbRes_SetReadError(-932);return 0;}return 2048;}
static int File_IsBusy(void){return io_busy;}
static int File_GetPendingCount(void){return pending;}
static void File_CancelRequests(void){assert(!"strict common boot must not free queue-owned buffers");}
static void *File_Request(s32 id,void *p){(void)id;(void)p;assert(!"unbounded request");return NULL;}
void *XbFile_RequestOwned(s32 id,void *p,s32 nominal){
 requests++;if(requests==fail_request){XbRes_SetReadError(-933);return NULL;}
 if(id==0x14b){assert(!p && !nominal);p=Heap_Alloc(2048,64,0,0);}
 else assert(id>=2 && id<=4 && p && nominal==2048);
 if(p)pending++;return p;
}
static int File_UpdateRequests(void){updates++;if(fail_update){XbRes_SetReadError(-934);return 0;}pending=0;return 1;}
void XbFile_AbortRequestsRetainBuffers(void){aborts++;pending=0;}
static void Snd_LoadBank(s32 mask,void *p,s32 sync){assert(mask==1 && p && sync==1);sounds++;if(fail_sound)XbRes_SetReadError(-935);}
/* ORIGINAL_FUNCTIONS */
static int converter(void *p,size_t n,int id){(void)p;(void)n;(void)id;assert(!"simulated IO does not convert real formats");return 0;}
static void release(void){if(gCommonRes){for(int i=0;i<3;i++)if(gCommonRes->data[i])Heap_Free(gCommonRes->data[i]);if(gCommonRes->boot)Heap_Free(gCommonRes->boot);Heap_Free(gCommonRes);gCommonRes=NULL;}assert(!live);}
static void reset(void){allocs=fail_alloc=requests=fail_request=pending=aborts=updates=fail_update=sounds=fail_sound=size_failure=boot_failure=io_busy=0;assert(!live && !gCommonRes);assert(!XbRes_BeginCheckedReads());}
int main(void){
 /* Missing converters refuse before IO/allocation beyond Common_Init. */
 reset();Common_Init();Common_LoadBoot();assert(XbRes_LastError() && live==1 && !gCommonRes->boot);release();XbRes_EndCheckedReads();
 assert(!XbRes_RegisterChecked(2,5,converter,0));assert(!XbRes_RegisterChecked(0x14b,0x14b,converter,0));
 for(int failure=1;failure<=5;failure++){
  reset();fail_alloc=failure;Common_Init();Common_LoadBoot();assert(XbRes_LastError());
  if(failure==1)assert(!gCommonRes && !live);
  else {assert(live==1 && !gCommonRes->boot);for(int i=0;i<3;i++)assert(!gCommonRes->data[i] && !gCommonRes->size[i]);}
  release();XbRes_EndCheckedReads();
 }
 for(int failure=0;failure<4;failure++){
  reset();Common_Init();boot_failure=failure==0;size_failure=failure?failure+1:0;Common_LoadBoot();
  assert(XbRes_LastError() && live==1 && !gCommonRes->boot);release();XbRes_EndCheckedReads();
 }
 reset();Common_Init();CommonRes *root=gCommonRes;Common_Init();assert(root==gCommonRes && live==1 && XbRes_LastError()==-909);release();XbRes_EndCheckedReads();
 reset();Common_Init();Common_LoadBoot();assert(live==5 && !XbRes_LastError());
 void *boot=gCommonRes->boot;Common_LoadBoot();assert(XbRes_LastError()==-909 && live==5 && gCommonRes->boot==boot);release();XbRes_EndCheckedReads();
 reset();Common_Reload();assert(XbRes_LastError()==-909 && !allocs && !requests);XbRes_EndCheckedReads();
 reset();XbRes_SetReadError(-936);Common_Init();Common_LoadBoot();Common_Reload();assert(!gCommonRes && !allocs && !requests && XbRes_LastError()==-936);XbRes_EndCheckedReads();
 for(int failure=0;failure<9;failure++){
  reset();Common_Init();Common_LoadBoot();assert(!XbRes_LastError() && live==5);
  void *owners[4]={gCommonRes->boot,gCommonRes->data[0],gCommonRes->data[1],gCommonRes->data[2]};
  if(failure<=4)fail_request=failure;else if(failure==5)fail_update=1;else if(failure==6)fail_sound=1;
  else if(failure==7)io_busy=1;else pending=1;
  Common_Reload();
  assert(live==5 && owners[0]==gCommonRes->boot);for(int i=0;i<3;i++)assert(owners[i+1]==gCommonRes->data[i]);
  if(!failure)assert(!XbRes_LastError() && requests==4 && sounds==1 && !pending && !aborts);
  else {assert(XbRes_LastError());if(failure>=7)assert(!requests && !aborts);else assert(aborts==1 && !pending);}
  release();XbRes_EndCheckedReads();
 }
 puts("PASS strict Common_Init/LoadBoot/Reload: allocation/read/queue/sound failure ownership, no unbounded requests; IO/heap/sound simulated; no original full boot");return 0;
}
