/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "port_compat.h"
#undef malloc
#include "xb_battle_runtime.h"
#include "xb_battle_fighter.h"
#include "xb_battle_stage.h"
#include "xb_typed_data.h"
#include "battle/btl_char_mgr.h"
#include "battle/btl_ai_think.h"
#include "battle/btl_obj.h"
#include "battle/btl_input.h"
#include "sys/pad.h"
#include "battle/battle_setup.h"
#include "sys/heap.h"
#include "sys/save.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>
extern AiThMgr *gBtlAi;
extern void BtlChar_ResetAll(void),BtlAiMgr_Reset(void);
extern void BtlChar_SetFlag(void *,int);
extern s32 Heap_GetFreeSize(void);
extern float D_002F2420[213];
void xsdk_log_write(const char *fmt,...){va_list ap;va_start(ap,fmt);vfprintf(stderr,fmt,ap);fputc('\n',stderr);va_end(ap);}
static void *load(const char *root,const char *name,size_t *n){char path[2048];assert(snprintf(path,sizeof(path),"%s/%s",root,name)<(int)sizeof(path));FILE *f=fopen(path,"rb");assert(f && !fseek(f,0,SEEK_END));long s=ftell(f);assert(s>0);rewind(f);*n=(size_t)s;void *p=malloc(*n);assert(p && fread(p,1,*n,f)==*n);fclose(f);return p;}
static int read_program(void *root,const char *name,uint8_t **bytes,size_t *n){*bytes=load(root,!strcmp(name,"DBZP.BIN")?"BIN/DBZP.BIN":name,n);return 0;}
int main(int argc,char **argv){
 setvbuf(stdout,NULL,_IOLBF,0);assert(argc==3);
 XbDataFiles files={read_program,argv[2]};int rc=XbTypedData_Load(&files,&gXbTypedMainCatalog,0);printf("typed data: %d\n",rc);assert(!rc && gPortDataStripped==2);
 size_t n2,n4,ns;void *c2=load(argv[1],"common-2.bin",&n2),*c4=load(argv[1],"common-4.bin",&n4),*stage=load(argv[1],"stage.bin",&ns);
 XbBattleRuntime *runtime=NULL;rc=XbBattleRuntime_Open(c2,n2,c4,n4,&runtime);printf("runtime: %d\n",rc);assert(!rc);free(c2);free(c4);s32 heap=Heap_GetFreeSize();
 XbBattleStageCollision *collision=NULL;assert(!XbBattleStage_PrepareCollision(stage,ns,&collision));assert(!XbBattleStage_PrepareStarts(collision,stage,ns));free(stage);assert(!XbBattleStage_InstallCollision(collision));
 for(unsigned pass=0;pass<2;pass++){
  assert(!XbTypedData_Load(&files,&gXbTypedMainCatalog,XBTD_VALIDATE_ONLY));assert(Heap_GetFreeSize()==heap);
  assert(!XbBattleFighter_BeginOriginalObjects());XbBattleFighter *fighters[2];
  for(unsigned side=0;side<2;side++){
   size_t mn,an;void *model=load(argv[1],side?"right-model.bin":"left-model.bin",&mn),*anim=load(argv[1],side?"right-animation.bin":"left-animation.bin",&an);XbFighterPrepared info;
   assert(!XbBattleFighter_PrepareModelAnimation(model,mn,anim,an,&fighters[side],&info));free(model);free(anim);
   assert(!XbBattleFighter_BindCommonReference(fighters[side],D_002F2420,213));int id=-1;
   assert(!XbBattleFighter_CreateModelObject(fighters[side],side,&id) && id==(int)side);
  }
  puts("objects ready; entering original roster/AI reset");
  rc=XbBattleRuntime_BeginManagers(runtime);printf("original managers: %d\n",rc);assert(!rc);
  assert(gBtlChars && gBtlChars->count==2 && gBtlAi && gBtlAi->data);
  XbBattleRuntimeInfo runtime_info;assert(!XbBattleRuntime_GetInfo(runtime,&runtime_info));
  assert(runtime_info.health[0]==40000 && runtime_info.health[1]==40000 && runtime_info.ai_level[1]==13);
  assert(XbBattleRuntime_BeginManagers(runtime)<0);
  for(unsigned side=0;side<2;side++){
   BtlMgrChr *chr=&gBtlChars->chars[side];assert(chr->side==(int)side && chr->objId==(int)side && chr->memberCount==1);
   assert(chr->members[0].present && chr->members[0].chara==(int)side && chr->members[0].cpuLevel==13);
   assert(chr->members[0].gauge.health>0 && chr->members[0].gauge.health==chr->members[0].gauge.healthMax);
   assert(gBtlAi->work[side].objId==(int)side && gBtlAi->work[side].param && gBtlAi->work[side].level==13);
   printf("side=%u hp=%d ki=%d AI=%d level=%d\n",side,chr->members[0].gauge.health,chr->members[0].gauge.ki,gBtlAi->work[side].type,gBtlAi->work[side].level);
  }
  assert(isfinite(gBtlAi->radiusSum) && gBtlAi->radiusSum>0);
  /* Isolated original input phase: a physical-pad fixture, then release.
   * Flag 2 normally comes from the battle sequence; no sequence is claimed. */
  BtlInputChr *input=(BtlInputChr *)&gBtlChars->chars[0];
  BtlChar_SetFlag(input,2);gPad[0].held=PAD_SQUARE;gPad[0].status=0;gPad[0].left[0]=0.5f;gPad[0].left[1]=0;
  BtlInput_Sample(input);assert(input->input.rec.buttons&BTLB_RUSH);
  BtlInput_Update(input);assert(input->input.held&BTLB_RUSH);assert(input->input.pressed&BTLB_RUSH);
  gPad[0].held=0;BtlInput_Sample(input);BtlInput_Update(input);assert(input->input.released&BTLB_RUSH);
  puts("PASS original input sample/remap/press/release with physical-pad fixture");
  BtlChar_ResetAll();BtlAiMgr_Reset();
  assert(!XbBattleRuntime_EndManagers(runtime) && !gBtlChars && !gBtlAi && !gSaveData);
  for(unsigned side=0;side<2;side++)XbBattleFighter_FreeModelAnimation(fighters[side]);
  assert(!XbBattleFighter_EndObjectDiagnostic());assert(Heap_GetFreeSize()==heap);
  printf("PASS original roster reset / AI init / reset / terminate: pass=%u heap restored\n",pass);
 }
 assert(!XbBattleStage_UninstallCollision(collision));assert(!XbBattleStage_FreeCollision(collision));assert(!XbBattleRuntime_Close(runtime));
 puts("PASS no Battle_Update/Battle_Main, GPU or SDK transport executed; marker stays 2");return 0;
}
