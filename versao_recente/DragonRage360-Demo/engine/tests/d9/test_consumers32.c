/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "port_compat.h"
#undef malloc
#include "xb_battle_common.h"
#include "battle/btl_char_mgr.h"
#include "sys/common.h"
#include <stddef.h>
#include <stdint.h>
#include "fixtures.h"
BtlCharMgr *gBtlChars;CommonRes *gCommonRes;
static unsigned char arena[1024*1024] __attribute__((aligned(32)));static size_t used;
void *malloc(size_t n){n=(n+31)&~31u;if(n>sizeof(arena)-used)return 0;void *p=arena+used;used+=n;return p;}
void free(void *p){(void)p;}
void *memcpy(void *d,const void *s,size_t n){unsigned char *a=d;const unsigned char *b=s;while(n--)*a++=*b++;return d;}
void *memset(void *p,int v,size_t n){unsigned char *a=p;while(n--)*a++=(unsigned char)v;return p;}
void *calloc(size_t n,size_t k){if(k && n>(size_t)-1/k)return 0;void *p=malloc(n*k);if(p)memset(p,0,n*k);return p;}
int memcmp(const void *a,const void *b,size_t n){const unsigned char *x=a,*y=b;while(n--){if(*x!=*y)return *x-*y;x++;y++;}return 0;}
#define CHECK(x) do {if(!(x))return __LINE__;}while(0)
int run(void){
 XbBattleCommon *c=0;XbBattleCommonInfo info;XbBattleConsumerProbe r;
 CHECK(!XbBattleCommon_Prepare(common2,sizeof(common2),&c,&info));
 CHECK(!XbBattleCommon_AttachItems(c,common4,sizeof(common4)));
 memset(common2,0,sizeof(common2));memset(common4,0,sizeof(common4));
 for(unsigned i=0;i<2;i++){
  CHECK(!XbBattleCommon_ProbeConsumers(c,&r));
  CHECK(r.animations==414 && r.voices==57 && r.characters==161 && r.items==4);
  CHECK(!gCommonRes && !gBtlChars);
 }
 CommonRes occupied={0};gCommonRes=&occupied;
 CHECK(XbBattleCommon_ProbeConsumers(c,&r)==-882 && gCommonRes==&occupied && !gBtlChars);
 CHECK(!r.animations && !r.voices && !r.characters && !r.items);
 gCommonRes=0;XbBattleCommon_Free(c);return 0;
}
__asm__(".global _start\n_start:\n call run\n mov %eax,%ebx\n mov $1,%eax\n int $0x80\n");
