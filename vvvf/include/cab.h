#pragma once

#include "rp.h"

#define CAB_MAIN_BG_Y (FB_H - (FB_H / 3U))
#define CAB_MAIN_BG_H (FB_H / 10U)

#define CAB_MAIN_DISP_Y (CAB_MAIN_BG_Y + CAB_MAIN_BG_H + 20U)
#define CAB_MAIN_DISP_X (10U)
#define CAB_MAIN_DISP_W (FB_W - (CAB_MAIN_DISP_X * 2U))
#define CAB_MAIN_DISP_H (FB_H - CAB_MAIN_DISP_Y - 10U)

#define CAB_OVERLAY_Y CAB_MAIN_BG_Y
#define CAB_OVERLAY_W 10U

void cab_draw_main(void);
void cab_draw_overlay(void);
void cab_draw_disp(u32_t const vvvf_lut_idx);
