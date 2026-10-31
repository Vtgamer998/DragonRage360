/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Real face DMA/VIF corpus. UNPACKs are tested ISOLATED with CL=WL=1 and
 * BASE/OFFSET/TOPS=0. MSCNT programs are NOT executed by this test. */
#define main previous_resource_validation
#include "real_iso_sanitize.c"
#undef main
#include "dr_face_vif.h"
#include "dr_vu1.h"
typedef struct {const unsigned char *source;unsigned unpacks;} Context;
static int isolated_transfer(void *opaque,const DrFaceVifCommand *c)
{
    Context *ctx=opaque;if(c->command!=0x6c)return 0;
    dr_vu1_reset(NULL,NULL);
    assert(!dr_vu1_unpack(c->command,c->num,c->immediate,ctx->source+c->data_offset,c->data_bytes));
    unsigned char copied[4096];size_t at=(c->immediate&1023)*16u,n=c->data_bytes;
    size_t first=n<16384-at?n:16384-at;
    assert(!dr_vu1_read((uint32_t)at,copied,first));
    if(first<n)assert(!dr_vu1_read(0,copied+first,n-first));
    assert(!memcmp(copied,ctx->source+c->data_offset,n));ctx->unpacks++;return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    FILE *f=fopen(argv[1],"rb");assert(f);assert(!fseeko(f,0,SEEK_END));off_t total=ftello(f);assert(total>0);
    dr_disc disc;assert(!dr_mount(&disc,file_read,f,(uint64_t)total));
    unsigned blocks=0,unpacks=0,vectors=0,continues=0,noops=0,tex_headers=0;
    for(unsigned ch=0;ch<161;ch++)for(unsigned skin=0;skin<8;skin++) {
        size_t size;unsigned char *pack=load(&disc,0x590+ch*10+skin,&size);unsigned count=dr_resource_le32(pack);
        for(unsigned i=0;i<count && i<=16;i++) {
            if(i!=0 && i<3)continue;
            size_t off,n;int r=dr_pack_memory_entry(pack,size,i,&off,&n,NULL);
            if(r==DR_NOT_FOUND) {continue;}assert(!r);const unsigned char *p=pack+off;
            if(n>=4 && (dr_resource_le32(p)&0xffff0000u)==0x60000000u) {
                Context ctx={p,0};DrFaceVifStats stats;size_t bad;
                assert(!dr_face_vif_parse(p,n,isolated_transfer,&ctx,&stats,&bad) && bad==SIZE_MAX);
                assert(ctx.unpacks==stats.unpacks);blocks++;unpacks+=stats.unpacks;vectors+=stats.vectors;
                continues+=stats.continues;noops+=stats.noops;
            } else {
                /* Header only. Full TexFile/pixel decoding is NOT claimed here. */
                assert(n>=0x60);uint32_t num=dr_resource_le32(p),table=dr_resource_le32(p+4);
                assert(num && num<=1024 && (uint64_t)table*4+(uint64_t)num*64<=n);tex_headers++;
            }
        }
        free(pack);
    }
    assert(blocks==2722 && unpacks==173798 && continues==86899 && noops==3659 && tex_headers==2148);
    printf("PASS real face corpus: DMA_VIF_blocks=%u UNPACKs=%u vectors=%u MSCNTs_parsed_not_executed=%u NOPs=%u texture_headers=%u\n",
           blocks,unpacks,vectors,continues,noops,tex_headers);
    fclose(f);return 0;
}
