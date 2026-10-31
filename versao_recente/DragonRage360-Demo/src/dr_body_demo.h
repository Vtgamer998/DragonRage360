/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DR_BODY_DEMO_H
#define DR_BODY_DEMO_H
#include "dr_body_pipeline.h"
/* Diagnostic only: one real model, original VU1 program, fixed reference pose,
 * real base-layer TEX0/STQ materials on the mesh. Two separately decoded
 * package texture previews. No second shading layer, animation or combat. */
int dr_body_demo_open(const dr_disc *,DrBodyPipelineStats *);
int dr_body_demo_open_face(const dr_disc *,int face,DrBodyPipelineStats *);
uint32_t dr_body_demo_face_mask(void);
int dr_body_demo_next_face(int current);
int dr_body_demo_draw(uint32_t width,uint32_t height);
int dr_body_demo_close(void);
unsigned dr_body_demo_material_count(void);
#endif
