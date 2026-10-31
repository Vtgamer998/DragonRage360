/* SPDX-License-Identifier: AGPL-3.0-or-later */
#define main previous_resource_tests
#include "real_iso_sanitize.c"
#undef main
#include "dr_body_pipeline.h"
#include "dr_texfile.h"
#include <math.h>
int main(int argc,char **argv)
{
    if(argc!=2 && argc!=3)return 2;
    FILE *f=fopen(argv[1],"rb");assert(f);assert(!fseeko(f,0,SEEK_END));off_t total=ftello(f);assert(total>0);
    dr_disc disc;assert(!dr_mount(&disc,file_read,f,(uint64_t)total));
    size_t cap=196608,n=0;DrGsVertex *v=malloc(cap*sizeof(*v));uint64_t *keys=malloc(cap/3*sizeof(*keys));
    unsigned char *rgba=malloc(4*1024*1024);assert(v && keys && rgba);DrBodyPipelineStats stats;
    assert(!dr_body_pipeline_materials(&disc,"pzs3us1/01423.bin",v,cap,keys,cap/3,&n,&stats));
    assert(n==13476 && stats.triangles==4492);uint64_t unique[64];unsigned count=0;
    FILE *dump=argc==3?fopen(argv[2],"wb"):NULL;
    if(argc==3) {
        assert(dump);uint32_t nv=(uint32_t)n;
        assert(fwrite(&nv,4,1,dump)==1 && fwrite(v,sizeof(*v),n,dump)==n && fwrite(keys,8,n/3,dump)==n/3);
    }
    for(size_t i=0;i<n;i++)assert(v[i].r==128 && v[i].g==128 && v[i].b==128 &&
        isfinite(v[i].s) && isfinite(v[i].t) && isfinite(v[i].q) && v[i].q!=0);
    for(size_t i=0;i<n/3;i++) {
        unsigned j=0;for(;j<count;j++)if(unique[j]==keys[i])break;
        if(j<count)continue;
        assert(count<64);unique[count++]=keys[i];
        DrTexFileImage image;uint32_t index;
        int rc=dr_texfile_body_material(&disc,"pzs3us1/01423.bin",keys[i],rgba,4*1024*1024,&image,&index);
        printf("TEX0=%016llx index=%u %ux%u PSM=%u rc=%d\n",(unsigned long long)keys[i],index,image.width,image.height,image.psm,rc);assert(!rc);
        if(dump) {
            assert(fwrite(&keys[i],8,1,dump)==1 && fwrite(&image.width,4,1,dump)==1 && fwrite(&image.height,4,1,dump)==1);
            assert(fwrite(rgba,1,image.rgba_bytes,dump)==image.rgba_bytes);
        }
    }
    printf("PASS body materials: triangles=%zu unique=%u; all TEX0 matched and decoded; finite STQ; CPU only\n",n/3,count);
    assert(count==13 && stats.face_mask==31);
    for(int face=0;face<8;face++) {
        int rc=dr_body_pipeline_face(&disc,"pzs3us1/01423.bin",v,cap,keys,cap/3,face,&n,&stats);
        if(rc==DR_NOT_FOUND) {
            assert(face>=5 && !n && !stats.triangles);printf("PASS facial slot=%d absent, rejected without published output\n",face);continue;
        }
        if(rc)fprintf(stderr,"face=%d rc=%d\n",face,rc);
        assert(!rc);
        unsigned char *pack;size_t pack_size,off,bytes;pack=load(&disc,0x590,&pack_size);
        assert(!dr_pack_memory_entry(pack,pack_size,11,&off,&bytes,NULL));
        for(size_t i=0;i<n/3;i++) {uint32_t index;assert(!dr_texfile_material(pack+off,bytes,keys[i],&index));}
        free(pack);
        for(size_t i=0;i<n;i++)assert(isfinite(v[i].s) && isfinite(v[i].t) && isfinite(v[i].q) && v[i].q!=0);
        printf("PASS facial substitution=%d triangles=%zu MSCNT=%u all materials resolved; reference pose only\n",face,n/3,stats.mscnt);
    }
    n=99;assert(dr_body_pipeline_materials(&disc,"pzs3us1/01423.bin",v,cap,keys,1,&n,&stats) && !n && !stats.triangles);
    if(dump)fclose(dump);
    free(rgba);free(keys);free(v);fclose(f);return 0;
}
