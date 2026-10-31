/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "xb_common_pack.h"
#include "xb_resconv.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
void XbRes_TestRead(int,int,uint64_t,uint64_t,void*,size_t);
static unsigned le32(const unsigned char*p){return p[0]|(unsigned)p[1]<<8|(unsigned)p[2]<<16|(unsigned)p[3]<<24;}
static unsigned native32(const unsigned char*p){unsigned x;memcpy(&x,p,4);return x;}
int main(int argc,char**argv){
 assert(argc==4&&!XbCommonPack_Register());const int ids[]={2,3,5};
 for(unsigned i=0;i<3;i++){
  FILE*f=fopen(argv[i+1],"rb");assert(f);assert(!fseek(f,0,SEEK_END));long n=ftell(f);rewind(f);assert(n>64);
  unsigned char*raw=malloc(n),*p=malloc(n),*before=malloc(n);assert(raw&&p&&before);assert(fread(raw,1,n,f)==(size_t)n);fclose(f);memcpy(p,raw,n);
  assert(!XbRes_BeginCheckedReads());XbRes_TestRead(1,ids[i]-1,0,n,p,n);assert(!XbRes_LastError());unsigned count=le32(raw);
  for(unsigned j=0;j<count+2;j++)assert(native32(p+j*4)==le32(raw+j*4));XbRes_EndCheckedReads();
  unsigned offsets[]={0,4,4+count*4,le32(raw+count*4)};
  for(unsigned j=0;j<4;j++){
   memcpy(p,raw,n);memset(p+offsets[j],0xff,4);memcpy(before,p,n);assert(!XbRes_BeginCheckedReads());XbRes_TestRead(1,ids[i]-1,0,n,p,n);assert(XbRes_LastError()&&!memcmp(p,before,n));XbRes_EndCheckedReads();
  }
  printf("PASS real common %d: %ld bytes, %u entries; whole-file strict hook, native relative header, 4 atomic refusals, host CPU only\n",ids[i],n,count);free(raw);free(p);free(before);
 }
 return 0;
}
