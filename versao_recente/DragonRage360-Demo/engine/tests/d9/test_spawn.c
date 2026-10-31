/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "xb_battle_stage.h"
#include "battle/col_a.h"
#include "battle/eft_det_b.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
StgColStage *gBtlStage;void *gStgColMesh;s32 gStgColMode;
s32 *gColMeshBase;s32 gColWalkCount,gColWalkMax;u16 *gColWalkBuf;
ColBox *gColWalkBox;ColMeshCb gColWalkCb;void *gColWalkCtx;
void Vec4_Copy(ColVec *d,ColVec *s){memmove(d,s,sizeof(*d));}
int main(int argc,char **argv){
 assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f && !fseek(f,0,SEEK_END));long n=ftell(f);assert(n>0);rewind(f);
 unsigned char *p=malloc((size_t)n);assert(p && fread(p,1,(size_t)n,f)==(size_t)n);fclose(f);
 XbBattleStageCollision *c=NULL;assert(!XbBattleStage_PrepareCollision(p,(size_t)n,&c));
 assert(!XbBattleStage_PrepareStarts(c,p,(size_t)n));
 assert(XbBattleStage_PrepareStarts(c,p,(size_t)n)==XB_STAGE_STATE);
 /* Corrupt MAPD offsets, overlap and NaN must fail transactionally. */
 unsigned char *bad=malloc((size_t)n);assert(bad);
 for(unsigned kind=0;kind<3;kind++){
  memcpy(bad,p,(size_t)n);unsigned map=(unsigned)bad[8]|(unsigned)bad[9]<<8|(unsigned)bad[10]<<16|(unsigned)bad[11]<<24;
  if(kind==0)memset(bad+map+44,0xff,4);
  if(kind==1)memcpy(bad+map+52,bad+map+44,4);
  if(kind==2){unsigned char nan[]={0,0,192,127};memcpy(bad+map+168,nan,4);}
  XbBattleStageCollision *other=NULL;assert(!XbBattleStage_PrepareCollision(bad,(size_t)n,&other));
  assert(XbBattleStage_PrepareStarts(other,bad,(size_t)n)<0);assert(!XbBattleStage_FreeCollision(other));
 }
 /* Valid floats with no solid ground must never manufacture a spawn. */
 memcpy(bad,p,(size_t)n);bad[16+224+3]=8;
 XbBattleStageCollision *no_floor=NULL;assert(!XbBattleStage_PrepareCollision(bad,(size_t)n,&no_floor));
 assert(!XbBattleStage_PrepareStarts(no_floor,bad,(size_t)n));free(bad);
 assert(!XbBattleStage_InstallCollision(no_floor));
 float misspos[4]={9,9,9,9},missrot[4]={9,9,9,9};int missing=9;
 assert(XbBattleStage_GetPlacement(no_floor,0,0,misspos,missrot,&missing)==DR_STAGE_BOUNDS && missing==-1);
 for(unsigned i=0;i<4;i++)assert(misspos[i]==0 && missrot[i]==0);
 assert(!XbBattleStage_UninstallCollision(no_floor));assert(!XbBattleStage_FreeCollision(no_floor));
 memset(p,0,(size_t)n);free(p);assert(!XbBattleStage_InstallCollision(c));
 for(unsigned kind=0;kind<3;kind++)for(unsigned side=0;side<2;side++){
  float pos[4],rot[4];int zone;
  assert(!XbBattleStage_GetPlacement(c,side,kind,pos,rot,&zone));
  assert(zone==0 && pos[1]==10 && pos[2]==0 && pos[3]==1 && rot[3]==1);
  assert(pos[0]==(kind==0?(side?10:-10):kind==1?0:5));
  assert(isfinite(rot[1]));
 }
 float pos[4]={9,9,9,9},rot[4]={9,9,9,9};int zone=9;
 assert(XbBattleStage_GetPlacement(c,2,0,pos,rot,&zone)==XB_STAGE_STATE);
 for(unsigned i=0;i<4;i++)assert(pos[i]==0 && rot[i]==0);assert(zone==-1);
 int native_zone=9;
 assert(XbStage_TryStartPlace(1,pos,rot,0,&native_zone)==1 && native_zone==0 && pos[0]==10);
 assert(XbStage_TryStartPlace(-1,pos,rot,0,&native_zone)==1 && native_zone==-1);
 assert(!XbBattleStage_UninstallCollision(c));
 assert(!XbStage_TryStartPlace(0,pos,rot,0,&native_zone));
 assert(!XbBattleStage_FreeCollision(c));
 puts("PASS: owned starts, alternative/special, original triangle ground, six placements, invalid side, cleanup");
 return 0;
}
