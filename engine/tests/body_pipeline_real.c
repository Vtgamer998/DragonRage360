/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Real pmdl + original ISO microprogram, reference VU execution, GIF triangles.
 * This is a bind-pose diagnostic, not the engine's battle loop. */
#define main previous_resource_validation
#include "real_iso_sanitize.c"
#undef main
#include "dr_body_pipeline.h"
#include "dr_body_model.h"
#include <math.h>
int main(int argc,char **argv)
{
    if(argc<2 || argc>3)return 2;
    FILE *f=fopen(argv[1],"rb");assert(f);assert(!fseeko(f,0,SEEK_END));off_t total=ftello(f);assert(total>0);
    dr_disc disc;assert(!dr_mount(&disc,file_read,f,(uint64_t)total));
    unsigned models=0,nodes=0,chains=0;
    for(unsigned ch=0;ch<161;ch++)for(unsigned skin=0;skin<8;skin++) {
        size_t pack_size;unsigned char *pack=load(&disc,0x590+ch*10+skin,&pack_size);
        if(pack_size==64 && dr_resource_le32(pack)==1 && dr_resource_le32(pack+4)==64 && dr_resource_le32(pack+8)==64) {
            for(size_t i=12;i<pack_size;i++)assert(!pack[i]);
            free(pack);continue;
        }
        size_t off,bytes;int r=dr_pack_memory_entry(pack,pack_size,2,&off,&bytes,NULL);assert(!r);
        if(bytes>=4 && !memcmp(pack+off,"pmdl",4)) {
            DrBodyPart parts[256];DrBodyModel model;size_t bad;
            assert(!dr_body_model_read(pack+off,bytes,parts,256,&model,&bad) && bad==SIZE_MAX);
            for(unsigned i=0;i<model.parts;i++)assert(parts[i].parent==UINT16_MAX || parts[i].parent<i);
            models++;nodes+=model.parts;chains+=model.chains;
        } else assert(bytes==64);
        free(pack);
    }
    assert(models==698 && nodes==43121 && chains==32454);
    printf("PASS body corpus: models=%u nodes=%u validated_body_VIF_chains=%u\n",models,nodes,chains);
    size_t capacity=196608,n=0;DrGsVertex *v=malloc(capacity*sizeof(*v));assert(v);
    DrBodyPipelineStats stats;int rc=dr_body_pipeline_build(&disc,"pzs3us1/01423.bin",v,capacity,&n,&stats);
    printf("body pipeline rc=%d vertices=%zu parts=%u chains=%u MSCNT=%u GIF=%u strips=%u triangles=%u ADC=%u instructions=%u\n",
        rc,n,stats.parts,stats.body_chains,stats.mscnt,stats.gif_packets,stats.strips,stats.triangles,stats.adc_vertices,stats.program_instructions);
    assert(!rc && n>0 && n%3==0 && stats.body_chains>0 && stats.program_instructions==127 && n/3==stats.triangles);
    float xmin=INFINITY,ymin=INFINITY,zmin=INFINITY,xmax=-INFINITY,ymax=-INFINITY,zmax=-INFINITY;
    for(size_t i=0;i<n;i++) {
        assert(isfinite(v[i].x) && isfinite(v[i].y) && isfinite(v[i].z) && v[i].z>=0 && v[i].z<=1);
        xmin=fminf(xmin,v[i].x);xmax=fmaxf(xmax,v[i].x);ymin=fminf(ymin,v[i].y);ymax=fmaxf(ymax,v[i].y);
        zmin=fminf(zmin,v[i].z);zmax=fmaxf(zmax,v[i].z);
    }
    printf("bounds x=%f..%f y=%f..%f z=%f..%f\n",xmin,xmax,ymin,ymax,zmin,zmax);
    if(argc==3) {FILE *out=fopen(argv[2],"wb");assert(out);assert(fwrite(v,sizeof(*v),n,out)==n);fclose(out);}
    n=99;memset(&stats,0xa5,sizeof(stats));
    assert(dr_body_pipeline_build(&disc,"pzs3us1/01423.bin",v,2,&n,&stats) && !n && !stats.triangles);
    puts("PASS output capacity failure: no published vertices or statistics");
    free(v);fclose(f);return 0;
}
