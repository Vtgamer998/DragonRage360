/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DR_BODY_PIPELINE_H
#define DR_BODY_PIPELINE_H
#include "dr_disc.h"
#include "dr_xenos.h"
typedef struct {uint32_t parts,body_chains,mscnt,gif_packets,strips,triangles,adc_vertices,program_instructions,face_mask;} DrBodyPipelineStats;
/* Development bind-pose viewer: real pmdl body + original VU1 program 0 from
 * the USA ISO -> reference interpreter -> packed GIF -> ordinary triangles.
 * Uses translation matrices that undo the two source pivots, not animation.
 * Parts get diagnostic colors; no game textures, GS effects or combat engine.
 * Single thread, uses global VU state. Output is partial on error and must be
 * discarded. Source ISO is read-only; no commercial resources are embedded. */
int dr_body_pipeline_build(const dr_disc *,const char *pack_path,DrGsVertex *,size_t capacity,
                           size_t *vertices,DrBodyPipelineStats *);
/* Same diagnostic pose, preserving STQ and one canonical TEX0 per triangle.
 * Material colors are neutral 128; no second shading layer or GS emulation. */
int dr_body_pipeline_materials(const dr_disc *,const char *,DrGsVertex *,size_t capacity,
                              uint64_t *triangle_tex0,size_t triangle_capacity,
                              size_t *vertices,DrBodyPipelineStats *);
/* face=-1 keeps pmdl's chain; 0..7 substitute node 0x30's chain from entries
 * 3..10 (original mouthA table). Reference pose only, no speech/lip playback. */
int dr_body_pipeline_face(const dr_disc *,const char *,DrGsVertex *,size_t capacity,
                         uint64_t *triangle_tex0,size_t triangle_capacity,int face,
                         size_t *vertices,DrBodyPipelineStats *);
#endif
