/* SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef DR_TEXFILE_H
#define DR_TEXFILE_H
#include "dr_disc.h"
#include <stddef.h>
#include <stdint.h>
typedef struct {uint32_t width,height,psm;size_t rgba_bytes;} DrTexFileImage;
/* Bounded TexFile subset: CT32 upload packets, CT32/PSMT8/PSMT4 base level,
 * CSM1 with CT32 palettes and CSA=0. Validates coverage before writing RGBA.
 * No relocation, mipmaps, GS state emulation or mapping to body materials. */
int dr_texfile_decode(const void *,size_t,uint32_t index,void *rgba,size_t capacity,DrTexFileImage *);
int dr_texfile_model_image(const dr_disc *,const char *pack_path,uint32_t index,
                          void *rgba,size_t capacity,DrTexFileImage *);
/* Match the relative TEX0 emitted by a model to one unique entry, including
 * TBP/CBP offsets; ignore only TCC/TFX and CLD, which are draw/cache state. */
int dr_texfile_material(const void *,size_t,uint64_t tex0,uint32_t *index);
int dr_texfile_body_material(const dr_disc *,const char *pack_path,uint64_t tex0,
                            void *rgba,size_t capacity,DrTexFileImage *,uint32_t *index);
#endif
