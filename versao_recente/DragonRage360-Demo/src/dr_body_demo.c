/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "dr_body_demo.h"
#include "dr_texfile.h"
#include <math.h>
#include <stdlib.h>
static DrGsVertex *mesh;
static size_t count;
static uint32_t materials[14];
static unsigned material_count;
static uint32_t face_mask;
static unsigned char *material_ids;
static uint32_t previews[2];
int dr_body_demo_close(void)
{
    int rc=dr_xenos_close();if(rc)return rc;
    free(mesh);free(material_ids);material_ids=NULL;mesh=NULL;count=0;material_count=0;face_mask=0;previews[0]=previews[1]=0;return 0;
}
int dr_body_demo_open_face(const dr_disc *disc,int face,DrBodyPipelineStats *stats)
{
    if(mesh)return DR_XENOS_STATE;
    size_t capacity=196608;DrGsVertex *v=malloc(capacity*sizeof(*v));if(!v)return DR_XENOS_FULL;
    size_t tri_cap=capacity/3;
    uint64_t *tex0=malloc(tri_cap*sizeof(*tex0));unsigned char *ids=malloc(tri_cap);
    if(!tex0 || !ids) {free(tex0);free(ids);free(v);return DR_XENOS_FULL;}
    size_t n=0;int rc=dr_body_pipeline_face(disc,"pzs3us1/01423.bin",v,capacity,tex0,tri_cap,face,&n,stats);
    if(rc) {free(tex0);free(ids);free(v);return rc;}
    uint64_t keys[14];unsigned number=0;
    for(size_t i=0;i<n/3;i++) {
        unsigned j=0;for(;j<number;j++)if(keys[j]==tex0[i])break;
        if(j==number) {
            if(number==14) {free(tex0);free(ids);free(v);return DR_XENOS_FULL;}
            keys[number++]=tex0[i];
        }
        ids[i]=(unsigned char)j;
    }
    free(tex0);
    float xmin=INFINITY,ymin=INFINITY,xmax=-INFINITY,ymax=-INFINITY;
    for(size_t i=0;i<n;i++) {
        xmin=fminf(xmin,v[i].x);xmax=fmaxf(xmax,v[i].x);ymin=fminf(ymin,v[i].y);ymax=fmaxf(ymax,v[i].y);
    }
    float extent=fmaxf(xmax-xmin,ymax-ymin);
    if(!isfinite(extent) || extent<=0) {free(ids);free(v);return DR_XENOS_INVALID;}
    float scale=470.0f/extent,cx=(xmin+xmax)*.5f,cy=(ymin+ymax)*.5f;
    /* Opposite orthographic diagnostic view, facing the face rather than the
     * back. This is a viewer transform, not the game's animated camera. */
    for(size_t i=0;i<n;i++) {v[i].x=640-(v[i].x-cx)*scale;v[i].y=375+(v[i].y-cy)*scale;v[i].z=1-v[i].z;}
    rc=dr_xenos_open();if(rc) {free(ids);free(v);return rc;}
    unsigned char *rgba=malloc(4*1024*1024);if(!rgba) {dr_xenos_close();free(ids);free(v);return DR_XENOS_FULL;}
    for(unsigned i=0;i<number && !rc;i++) {
        DrTexFileImage image;uint32_t index;
        rc=dr_texfile_body_material(disc,"pzs3us1/01423.bin",keys[i],rgba,4*1024*1024,&image,&index);
        if(!rc)rc=dr_xenos_texture_create(image.width,image.height,rgba,&materials[i]);
    }
    for(unsigned i=0;i<2 && !rc;i++) {
        DrTexFileImage image;
        rc=dr_texfile_model_image(disc,"pzs3us1/01423.bin",i,rgba,4*1024*1024,&image);
        if(!rc)rc=dr_xenos_texture_create(image.width,image.height,rgba,&previews[i]);
    }
    free(rgba);
    if(rc) {dr_xenos_close();free(ids);free(v);return rc;}
    mesh=v;material_ids=ids;count=n;material_count=number;face_mask=stats->face_mask;return 0;
}
int dr_body_demo_draw(uint32_t width,uint32_t height)
{
    if(!mesh)return DR_XENOS_STATE;
    const DrXenosDraw draw={.logical_width=1280,.logical_height=720,.depth=2,.depth_write=1,.blend=1,
        .scissor={0,0,(int)width,(int)height}};
    /* The arena is 1024 vertices: end/fence/wait before every reuse. No resolves
     * between chunks, preserving color and depth in eDRAM. This is deliberately
     * a slow diagnostic path, not a measured battle rendering implementation. */
    for(size_t at=0;at<count;) {
        uint32_t n=(uint32_t)(count-at);if(n>1023)n=1023;
        int rc=dr_xenos_begin(width,height);if(rc)return rc;
        for(size_t at_run=at;at_run<at+n && !rc;) {
            size_t end_run=at_run+3;unsigned material=material_ids[at_run/3];
            while(end_run<at+n && material_ids[end_run/3]==material)end_run+=3;
            rc=dr_xenos_draw(mesh+at_run,(uint32_t)(end_run-at_run),materials[material],&draw);
            at_run=end_run;
        }
        int end=dr_xenos_end();
        if(rc || end)return rc?rc:end;
        at+=n;
    }
    int rc=dr_xenos_begin(width,height);if(rc)return rc;
    DrXenosDraw image_draw=draw;image_draw.depth=1;image_draw.depth_write=0;image_draw.blend=1;
    for(unsigned i=0;i<2 && !rc;i++) {
        float x=950,y=210+(float)i*190,w=160,h=160;
        const DrGsVertex quad[]={
            {x,y,.5f,128,128,128,128,0,0,1},{x+w,y,.5f,128,128,128,128,1,0,1},{x,y+h,.5f,128,128,128,128,0,1,1},
            {x,y+h,.5f,128,128,128,128,0,1,1},{x+w,y,.5f,128,128,128,128,1,0,1},{x+w,y+h,.5f,128,128,128,128,1,1,1}};
        rc=dr_xenos_draw(quad,6,previews[i],&image_draw);
    }
    int end=dr_xenos_end();return rc?rc:end;
}

unsigned dr_body_demo_material_count(void) {return material_count;}

uint32_t dr_body_demo_face_mask(void) {return face_mask;}
int dr_body_demo_open(const dr_disc *disc,DrBodyPipelineStats *stats) {return dr_body_demo_open_face(disc,-1,stats);}

int dr_body_demo_next_face(int current)
{
    if(!mesh || current< -1 || current>7)return -1;
    for(unsigned i=0;i<9;i++) {
        current=(current+2)%9-1;
        if(current<0 || (face_mask&(1u<<(unsigned)current)))return current;
    }
    return -1;
}
