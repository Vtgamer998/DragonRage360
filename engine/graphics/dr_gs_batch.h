/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DR_GS_BATCH_H
#define DR_GS_BATCH_H
#include "dr_xenos.h"
/* Consecutive descriptors only. Vertex spans must belong to the same owned
 * replay arena; VU uniforms must contain 76 readable floats. No GPU calls.
 * Returns 1 after extending left.count, otherwise leaves both inputs intact.
 * Full native preflight still follows, including every vertex. */
int dr_gs_batch_merge(DrXenosTargetCommand *left,const DrXenosTargetCommand *right);
#endif
