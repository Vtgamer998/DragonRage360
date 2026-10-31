/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "xb_resconv.h"
#include "xb_common_items.h"
#include "sys/common.h"
#include "battle/view_b.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
CommonRes *gCommonRes;
void XbRes_TestRead(int,int,uint64_t,uint64_t,void *,size_t);
static unsigned le16(const unsigned char *p){return p[0]|(unsigned)p[1]<<8;}
static unsigned le32(const unsigned char *p){return le16(p)|le16(p+2)<<16;}
int main(int argc,char **argv){
 assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f);assert(!fseek(f,0,SEEK_END));long n=ftell(f);rewind(f);assert(n>64);
 unsigned char *raw=malloc(n),*p=malloc(n),*bad=malloc(n);assert(raw && p && bad);assert(fread(raw,1,n,f)==(size_t)n);fclose(f);
 assert(!XbRes_RegisterCommonItems() && XbRes_HasCheckedConverter(4) && !XbRes_HasCheckedConverter(2));
 memcpy(p,raw,n);assert(!XbRes_BeginCheckedReads());XbRes_TestRead(1,3,0,n,p,n);assert(!XbRes_LastError() && XbRes_SourceSize(p)==(size_t)n);
 CommonRes common={0};common.data[2]=p;common.size[2]=n;gCommonRes=&common;
 unsigned a=le32(raw+4)&~3u,b=le32(raw+8)&~3u,nc=(b-a)/60,ni=(n-b)/40;assert(nc==161 && ni==350);
 for(unsigned i=0;i<nc;i++){
  int costume=99;assert(ChrTbl_WrapCostume(i,&costume)==(int)le16(raw+a+i*60+10) && !costume);
  assert(ChrTbl_GetCost(i)==(int)le16(raw+a+i*60+12));
  u16 ids[ITEMSET_COUNT]={0};s32 stats[5],ability[4];ItemSet_GetStats(ids,stats,ability,i);assert(stats[4]==(int)le32(raw+a+i*60));
 }
 for(unsigned i=0;i<ni;i++){
  const unsigned char *item=raw+b+i*40;u16 ids[ITEMSET_COUNT]={0};s32 stats[5],ability[4];ids[0]=i+1;
  ItemSet_GetStats(ids,stats,ability,-1);
  for(unsigned j=0;j<4;j++){
   assert(stats[j]==(item[0]==ITEMTBL_TYPE_AI?0:(short)le16(item+12+j*2)));
   assert((unsigned)ability[j]==(item[0]==ITEMTBL_TYPE_AI?0:le32(item+24+j*4)));
  }
  assert(stats[4]==(item[0]==ITEMTBL_TYPE_AI?(int)(i+1)-ITEMTBL_AI_FIRST:0));
 }
 gCommonRes=NULL;XbRes_EndCheckedReads();
 unsigned offsets[]={0,4,8,12,a,a+10,a+44,a+48,b+40*(ni-1)};
 for(unsigned i=0;i<sizeof(offsets)/sizeof(offsets[0]);i++){
  memcpy(p,raw,n);memset(p+offsets[i],0xff,4);if(offsets[i]==a+44)p[offsets[i]]=161;memcpy(bad,p,n);
  assert(!XbRes_BeginCheckedReads());XbRes_TestRead(1,3,0,n,p,n);
  assert(XbRes_LastError() && !memcmp(p,bad,n));XbRes_EndCheckedReads();
 }
 free(raw);free(p);free(bad);
 puts("PASS real common4 File post-read conversion -> original readers: 161 characters, 350 items, 9 atomic corruptions; host CPU only, no fight");return 0;
}
