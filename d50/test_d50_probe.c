/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Host test for D50 milestones 1 and 2. Uses the GPU call/lifetime mocks from
 * test_xenos.c; no rasterization, no PowerPC, no console claim. */
#define main previous_lifetime_tests
#include "test_xenos.c"
#undef main
#include "gfx_probe.h"
#include "xsdk_compat.h"
#include <assert.h>
#include <string.h>
int main(void)
{
    previous_lifetime_tests();
    /* Milestone 1: file, memory, time and log. */
    D50Buffer missing={0};
    assert(D50_ReadFile("/nonexistent/d50-probe-input",&missing)==-1);
    assert(missing.data==NULL && missing.size==0);
    assert(D50_ReadFile(NULL,&missing)==-1);
    void *block=D50_Alloc(64);
    assert(block && ((unsigned char *)block)[0]==0 && ((unsigned char *)block)[63]==0);
    D50_Release(block);
    assert(D50_Alloc(0)==NULL);
    uint64_t t0=D50_NowUs(),t1=D50_NowUs();
    assert(t0<=t1);
    /* Milestone 2: clear plus one triangle through dr_xenos, validated by the
     * same preflight the title uses. */
    unsigned before=draws;
    assert(D50_GfxProbeRun(640,480)==0);
    assert(draws==before+1);
    assert(last_primitive==XSDK_PRIMITIVE_TRIANGLE_LIST);
    assert(vertex_bytes==3*12*4);
    /* Invalid sizes are refused before touching the GPU. */
    before=draws;
    assert(D50_GfxProbeRun(100,480)!=0 && draws==before);
    assert(D50_GfxProbeRun(640,0)!=0 && draws==before);
    /* 1280x720 is the supported maximum. */
    before=draws;
    assert(D50_GfxProbeRun(1280,720)==0 && draws==before+1);
    D50_Log("PASS: D50 milestones 1-2 (compat layer + clear/triangle probe), host mocks only");
    return 0;
}
