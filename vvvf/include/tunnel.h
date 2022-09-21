#pragma once

#define TUNNEL_ANIM_X_SHIFT_TOGGLE 1
#define TUNNEL_ANIM_Y_SHIFT_TOGGLE 0
#define TUNNEL_ANIM_X_LOOK_TOGGLE 1
#define TUNNEL_ANIM_Y_LOOK_TOGGLE 1

#include "audio.h"
#include "cab.h"
#include "fb.h"

#define TUNNEL_ADDR (AUDIO_ADDR + AUDIO_SIZ)

#define TUNNEL_W FB_W
#define TUNNEL_H CAB_MAIN_BG_Y
#define TUNNEL_Y 0U
#define TUNNEL_X 0U

#define TUNNEL_TEX_ADDR TUNNEL_ADDR
#define TUNNEL_TEX_W 256U
#define TUNNEL_TEX_H 256U
#define TUNNEL_TEX_D 3U
#define TUNNEL_TEX_SIZ (TUNNEL_TEX_W * TUNNEL_TEX_H * TUNNEL_TEX_D)

#define TUNNEL_LUT_W (TUNNEL_W * 2U)
#define TUNNEL_LUT_H (TUNNEL_H * 2U)
#define TUNNEL_LUT_D 2U
#define TUNNEL_LUT_LEN (TUNNEL_LUT_W * TUNNEL_LUT_H)
#define TUNNEL_LUT_SIZ (TUNNEL_LUT_LEN * TUNNEL_LUT_D)
#define TUNNEL_DPH_ADDR (TUNNEL_TEX_ADDR + TUNNEL_LUT_SIZ)
#define TUNNEL_ANG_ADDR (TUNNEL_DPH_ADDR + TUNNEL_LUT_SIZ)
#define TUNNEL_SHD_ADDR (TUNNEL_ANG_ADDR + TUNNEL_LUT_SIZ)

#define TUNNEL_SIZ (TUNNEL_TEX_SIZ + (3U * TUNNEL_LUT_SIZ))

_Static_assert(TUNNEL_TEX_ADDR % 4U == 0U, "Tunnel texture not 4b aligned.");
_Static_assert(TUNNEL_DPH_ADDR % 4U == 0U, "Tunnel depth LUT not 4b aligned.");
_Static_assert(TUNNEL_ANG_ADDR % 4U == 0U, "Tunnel angle LUT not 4b aligned.");
_Static_assert(TUNNEL_SHD_ADDR % 4U == 0U, "Tunnel shade LUT not 4b aligned.");

void tunnel_calc_tex_audio(void);
void tunnel_calc_tex_subway(void);
void tunnel_calc_lut(void);
void tunnel_draw(float const anim);
void tunnel_draw_ang(void);
void tunnel_draw_dph(void);
void tunnel_draw_tex(void);
void tunnel_draw_shd(void);
