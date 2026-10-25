/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Real ISO, full loader/decoder/VU/viewer; only GPU calls are mocked. */
#define _FILE_OFFSET_BITS 64
#define _POSIX_C_SOURCE 200809L
#define main previous_native_tests
#include "test_xenos.c"
#undef main
#define main previous_resource_tests
#include "real_iso_sanitize.c"
#undef main
#include "dr_body_demo.h"
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    FILE *f=fopen(argv[1],"rb");assert(f);assert(!fseeko(f,0,SEEK_END));off_t total=ftello(f);assert(total>0);
    dr_disc disc;assert(!dr_mount(&disc,file_read,f,(uint64_t)total));
    for(int face=-1;face<5;face++) {
        DrBodyPipelineStats stats;int rc=dr_body_demo_open_face(&disc,face,&stats);
        printf("real viewer face=%d open=%d materials=%u\n",face,rc,dr_body_demo_material_count());fflush(stdout);
        assert(!rc && dr_body_demo_material_count()<=14 && dr_body_demo_face_mask()==31);
        assert(!dr_body_demo_draw(1280,720));assert(!dr_body_demo_close());
        assert(texture_frees==texture_creates);
    }
    fclose(f);puts("PASS real viewer: reference + five faces, real materials, batching and cleanup; GPU mocked, no rasterization");return 0;
}
