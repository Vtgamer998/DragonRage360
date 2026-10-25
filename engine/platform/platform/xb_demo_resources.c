/* SPDX-License-Identifier: AGPL-3.0-or-later */
#include "xb_resconv.h"
#include "xb_demo_resources.h"
#include "dr_resource.h"
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define XB_NATIVE_BIG_ENDIAN 1
#else
#define XB_NATIVE_BIG_ENDIAN 0
#endif
static int animation(void *buf, size_t n, int id) {
    (void)id; return dr_anim_prepare(buf,n,XB_NATIVE_BIG_ENDIAN);
}
static int skeleton(void *buf, size_t n, int id) {
    size_t nodes=0;
    (void)id; return dr_skeleton_prepare(buf,n,XB_NATIVE_BIG_ENDIAN,&nodes);
}
int XbRes_RegisterAnimation(int first, int last, int packed) {
    return XbRes_RegisterChecked(first,last,animation,packed);
}
int XbRes_RegisterSkeleton(int first, int last, int packed) {
    return XbRes_RegisterChecked(first,last,skeleton,packed);
}
