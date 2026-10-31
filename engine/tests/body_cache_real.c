/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "dr_body_pipeline.h"
#include "dr_body_demo.h"
#include "dr_texfile.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
static FILE*iso;static dr_span model;static unsigned model_reads,reads;static int forbidden;
static int reader(void*ctx,uint64_t at,void*out,size_t n){(void)ctx;assert(!forbidden);reads++;if(at==model.offset&&n==model.size)model_reads++;return fseeko(iso,(off_t)at,SEEK_SET)||fread(out,1,n,iso)!=n?-1:0;}
static unsigned textures,uploads,draw_vertices,draws;static int close_failure;
int dr_xenos_open(void){assert(!textures);return 0;}
int dr_xenos_close(void){if(close_failure)return DR_XENOS_GPU;textures=0;return 0;}
int dr_xenos_texture_create(uint32_t w,uint32_t h,const void*p,uint32_t*out){assert(w&&h&&p&&out);*out=++textures;uploads++;return 0;}
int dr_xenos_begin(uint32_t w,uint32_t h){assert(w==1280&&h==720);return 0;}
int dr_xenos_end(void){return 0;}
int dr_xenos_draw(const DrGsVertex*v,uint32_t n,uint32_t texture,const DrXenosDraw*d){assert(v&&n&&n<=1023&&n%3==0&&texture&&texture<=textures&&d);for(unsigned i=0;i<n;i++)assert(isfinite(v[i].x)&&isfinite(v[i].y));draw_vertices+=n;draws++;return 0;}
int dr_body_pipeline_reference_face(const dr_disc*,const char*,DrGsVertex*,size_t,uint64_t*,size_t,int,size_t*,DrBodyPipelineStats*);
static void pipeline(dr_disc*d){
 enum{CAP=196608};DrGsVertex*base=malloc(CAP*sizeof(*base)),*v=malloc(CAP*sizeof(*v)),*gold=malloc(CAP*sizeof(*gold));uint64_t*basekeys=malloc(CAP/3*sizeof(*basekeys)),*keys=malloc(CAP/3*sizeof(*keys)),*goldkeys=malloc(CAP/3*sizeof(*goldkeys));assert(base&&v&&gold&&basekeys&&keys&&goldkeys);
 model_reads=0;DrBodySource*s=NULL;assert(!dr_body_source_open(d,"pzs3us1/01423.bin",&s)&&model_reads==1);unsigned after=reads;DrBodyFaceSpan span;size_t n=0,gn=0,fn;DrBodyPipelineStats stats,gs;
 forbidden=1;assert(!dr_body_source_build(s,base,CAP,basekeys,CAP/3,-1,0,&n,&stats,&span)&&span.present&&reads==after);forbidden=0;
 assert(!dr_body_pipeline_reference_face(d,"pzs3us1/01423.bin",gold,CAP,goldkeys,CAP/3,-1,&gn,&gs));assert(n==gn&&!memcmp(base,gold,n*sizeof(*base))&&!memcmp(basekeys,goldkeys,n/3*sizeof(*basekeys))&&!memcmp(&stats,&gs,sizeof(stats)));
 for(int face=0;face<5;face++){
  after=reads;forbidden=1;assert(!dr_body_source_build(s,v,CAP,keys,CAP/3,face,1,&fn,&stats,NULL)&&stats.body_chains==1&&reads==after);forbidden=0;
  assert(!dr_body_pipeline_reference_face(d,"pzs3us1/01423.bin",gold,CAP,goldkeys,CAP/3,face,&gn,&gs));assert(gn==n-span.count+fn);
  size_t a=span.first,z=n-a-span.count;
  assert(!memcmp(base,gold,a*sizeof(*base))&&!memcmp(v,gold+a,fn*sizeof(*v))&&!memcmp(base+a+span.count,gold+a+fn,z*sizeof(*base)));
  assert(!memcmp(basekeys,goldkeys,a/3*sizeof(*keys))&&!memcmp(keys,goldkeys+a/3,fn/3*sizeof(*keys))&&!memcmp(basekeys+(a+span.count)/3,goldkeys+(a+fn)/3,z/3*sizeof(*keys)));
 }
 dr_body_source_free(s);free(base);free(v);free(gold);free(basekeys);free(keys);free(goldkeys);
 puts("PASS: cached source/face-only geometry and TEX0 match D33 full-body interpreter exactly for all 5 real expressions; no ISO reads after source open");
}
int main(int argc,char**argv){
 assert(argc==2);iso=fopen(argv[1],"rb");assert(iso&&!fseeko(iso,0,SEEK_END));uint64_t bytes=(uint64_t)ftello(iso);dr_disc d;assert(!dr_mount(&d,reader,NULL,bytes));assert(!dr_game_file(&d,"pzs3us1/01423.bin",&model));pipeline(&d);
 model_reads=uploads=0;DrBodyPipelineStats s;assert(!dr_body_demo_open(&d,&s)&&model_reads==1&&textures==15&&uploads==15);DrBodyDemoProfile p;dr_body_demo_profile(&p);assert(p.body_builds==1&&p.model_reads==1&&p.texture_uploads==15&&p.source_bytes==model.size);
 DrBodyDemoProbe probe;assert(!dr_body_demo_probe(0,&probe)&&(probe.argb&0xffffff));
 forbidden=1;unsigned after=reads;for(int face=0;face<5;face++){
  assert(!dr_body_demo_select_face(face,&s)&&s.body_chains==1);dr_body_demo_profile(&p);assert(!p.face_cache_hit&&p.body_builds==1&&p.face_builds==(unsigned)face+1&&uploads==15&&reads==after);
  assert(!dr_body_demo_draw(1280,720));
 }
 for(int face=0;face<5;face++){assert(!dr_body_demo_select_face(face,&s));dr_body_demo_profile(&p);assert(p.face_cache_hit&&p.face_builds==5&&p.body_builds==1&&reads==after&&uploads==15);}
 assert(dr_body_demo_select_face(7,&s));assert(!dr_body_demo_select_face(-1,&s)&&s.body_chains==53);draw_vertices=draws=0;assert(!dr_body_demo_draw(1280,720)&&draw_vertices==4492*3+12);
 close_failure=1;assert(dr_body_demo_close()==DR_XENOS_GPU);assert(!dr_body_demo_select_face(0,&s));close_failure=0;assert(!dr_body_demo_close()&&!textures);forbidden=0;
 assert(!dr_body_demo_open(&d,&s)&&model_reads==2&&!dr_body_demo_close());fclose(iso);
 puts("PASS: real viewer reads pack once; 15 uploads total; expressions reuse body/textures, lazy cache hits need no VU/ISO; close timeout retains ownership; reopen reloads once; GPU calls mocked");
}
