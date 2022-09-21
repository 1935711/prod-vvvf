#pragma once

#include "vvvf.h"

typedef struct shader_lut_item_s
{
    float const a;
    float const b;
} shader_lut_item_st;

#define SHADER_ADDR (VVVF_ADDR + VVVF_SIZ)

#define SHADER_RAYMARCH_FRAME_ADDR SHADER_ADDR
#define SHADER_RAYMARCH_FRAME_COUNT 158U
#define SHADER_RAYMARCH_FRAME_DIV (4U)
#define SHADER_RAYMARCH_FRAME_W (FB_W / SHADER_RAYMARCH_FRAME_DIV)
#define SHADER_RAYMARCH_FRAME_H (FB_H / SHADER_RAYMARCH_FRAME_DIV)
#define SHADER_RAYMARCH_FRAME_LEN                                              \
    (SHADER_RAYMARCH_FRAME_W * SHADER_RAYMARCH_FRAME_H *                       \
     SHADER_RAYMARCH_FRAME_COUNT)
#define SHADER_RAYMARCH_FRAME_D 1U
#define SHADER_RAYMARCH_FRAME_SIZ                                              \
    (SHADER_RAYMARCH_FRAME_LEN * SHADER_RAYMARCH_FRAME_D)

#define SHADER_WAVE_FRAME_DIV (4U)
#define SHADER_WAVE_FRAME_W (FB_W / SHADER_WAVE_FRAME_DIV)
#define SHADER_WAVE_FRAME_H (FB_H / SHADER_WAVE_FRAME_DIV)

#define SHADER_FIELD_FRAME_DIV (4U)
#define SHADER_FIELD_FRAME_W (FB_W / SHADER_FIELD_FRAME_DIV)
#define SHADER_FIELD_FRAME_H (FB_H / SHADER_FIELD_FRAME_DIV)

#define SHADER_MALINA_FRAME_ADDR                                               \
    (SHADER_RAYMARCH_FRAME_ADDR + SHADER_RAYMARCH_FRAME_SIZ)
#define SHADER_MALINA_FRAME_COUNT 80U
#define SHADER_MALINA_FRAME_DIV 4U
#define SHADER_MALINA_FRAME_D 3U
#define SHADER_MALINA_FRAME_W (FB_W / SHADER_MALINA_FRAME_DIV)
#define SHADER_MALINA_FRAME_H (FB_H / SHADER_MALINA_FRAME_DIV)
#define SHADER_MALINA_FRAME_LEN                                                \
    (SHADER_MALINA_FRAME_W * SHADER_MALINA_FRAME_H * SHADER_MALINA_FRAME_COUNT)
#define SHADER_MALINA_FRAME_SIZ                                                \
    (SHADER_MALINA_FRAME_LEN * SHADER_MALINA_FRAME_D)

#define SHADER_MALINA_LEAVES_FRAME_ADDR                                        \
    (SHADER_MALINA_FRAME_ADDR + SHADER_MALINA_FRAME_SIZ)
#define SHADER_MALINA_LEAVES_FRAME_D (3U)
#define SHADER_MALINA_LEAVES_FRAME_DIV SHADER_MALINA_FRAME_DIV
#define SHADER_MALINA_LEAVES_FRAME_W (FB_W / SHADER_MALINA_LEAVES_FRAME_DIV)
#define SHADER_MALINA_LEAVES_FRAME_H (FB_H / SHADER_MALINA_LEAVES_FRAME_DIV)
#define SHADER_MALINA_LEAVES_FRAME_LEN                                         \
    (SHADER_MALINA_LEAVES_W * SHADER_MALINA_LEAVES_H)
#define SHADER_MALINA_LEAVES_FRAME_SIZ                                         \
    (SHADER_MALINA_LEAVES_LEN * SHADER_MALINA_LEAVES_D)

#define SHADER_SIZ (SHADER_MALINA_LEAF_SIZ)

void shader_raymarch_draw(float const time, u32_t const frame_idx);
void shader_wave_draw(float const time);
void shader_field_draw(float const time);
void shader_malina_draw(float const time, u32_t const frame_idx);
void shader_raymarch_calc_frames(float const time_delta);
void shader_malina_calc_frames(float const time_delta);
void shader_malina_calc_leaves(void);
