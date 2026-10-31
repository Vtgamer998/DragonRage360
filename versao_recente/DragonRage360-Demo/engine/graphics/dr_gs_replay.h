/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DR_GS_REPLAY_H
#define DR_GS_REPLAY_H
#include "gs_draw.h" /* The pinned engine's actual ABI, not a copied descriptor. */
#include "dr_xenos.h"
enum { DR_GS_UNSUPPORTED=-200,DR_GS_TEXTURE=-201,DR_GS_MEMORY=-202 };
typedef struct {
    const float *vu_vertices;
    const uint32_t *vu_indices;
    const Vu0Uniform *vu_uniforms;
    uint32_t vu_vertex_count,vu_index_count,vu_uniform_count;
    float depth_max;
    uint32_t physical_width,physical_height,logical_width,logical_height;
    int target; /* The single GS target represented by this diagnostic pass. */
    uint32_t saved_targets; /* 1: use created native saved-target IDs from each GsDraw.target. */
    uint32_t white_texture;
    uint32_t movie_texture[2]; /* pre-uploaded immutable VRAM snapshots for native 100/101 */
    const uint32_t *pipeline_keys; /* Indexed by GsDraw.pipeline, from pipeGet. */
    uint32_t pipeline_count;
    /* Resolve an engine handle to an existing dr_xenos texture. 0 = unavailable.
     * Called during preflight; must not issue GPU commands or change renderer state. */
    uint32_t (*texture)(GsTex,void *);
    void *opaque;
} DrGsReplay;
/* Replay actual gs_draw descriptors into a native diagnostic surface, or created
 * native saved color/depth targets when saved_targets=1. GsBackend registration,
 * allocation and present are owned by dr_gs_backend. Indexed native VU
 * programs and shadow stencil runs are supported with complete source arenas.
 * Unsupported state rejects the ENTIRE list before begin. Input errors detected
 * by the native core reject the list before any primitive is emitted.
 * failed_draw receives the offending index; UINT32_MAX denotes a batch error.
 * dr_xenos_open must already have succeeded, with no active batch.
 * One game/render thread only; this does not synchronize competing callers. */
int dr_gs_replay(const DrGsReplay *,const GsDraw *,uint32_t draw_count,
                 const Vtx *,uint32_t vertex_count,uint32_t *failed_draw);
#endif
