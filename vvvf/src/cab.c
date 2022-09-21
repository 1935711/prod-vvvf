#include "cab.h"
#include "fb.h"
#include "text.h"
#include "vvvf.h"

void cab_draw_main(void)
{
    for (u16_t disp_h = 0U; disp_h < FB_H - CAB_MAIN_DISP_Y; ++disp_h)
    {
        for (u16_t w = 0U; w < FB_W; ++w)
        {
            u8_t const color = (10U + disp_h) > 30 ? 30U : 10U + disp_h;
            u16_t const x = w;
            u16_t const y = CAB_MAIN_DISP_Y + disp_h;
            FB_DRAW_PIX(FB_BUF_0, x, y, color, color, color);
            FB_DRAW_PIX(FB_BUF_1, x, y, color, color, color);
        }
    }
    for (u16_t bg_h = 0U; bg_h < CAB_MAIN_BG_H; ++bg_h)
    {
        for (u16_t w = 0U; w < FB_W / 2U; ++w)
        {
            u8_t const color = 100 - (w / 2) < 30 ? 30U : 100 - (w / 2U);
            u16_t const x0 = w;
            u16_t const x1 = FB_W - 1U - w;
            u16_t const y = CAB_MAIN_BG_Y + bg_h;
            FB_DRAW_PIX(FB_BUF_0, x0, y, color, color, color);
            FB_DRAW_PIX(FB_BUF_0, x1, y, color, color, color);
            FB_DRAW_PIX(FB_BUF_1, x0, y, color, color, color);
            FB_DRAW_PIX(FB_BUF_1, x1, y, color, color, color);
        }
    }
}

void cab_draw_overlay(void)
{
    for (u16_t sides_h = 0U; sides_h < CAB_MAIN_BG_Y; ++sides_h)
    {
        for (u16_t w = 0U; w < sides_h / 2U; ++w)
        {
            u8_t const color = ((100 - w) < 30 ? 30U : 100U - w);
            FB_DRAW_PIX(FB_BUF_A, w, sides_h, color, color, color);
            FB_DRAW_PIX(FB_BUF_A, FB_W - w - 1U, sides_h, color, color, color);
        }
    }
}

void cab_draw_disp(u32_t const vvvf_lut_idx)
{
    vvvf_lut_item_st const vvvf_lut =
        (*((volatile vvvf_lut_item_st(*)[VVVF_LUT_LEN])
               VVVF_LUT_ADDR))[vvvf_lut_idx];

    u8_t const gap = 20U;
    for (u16_t y = 0U; y < CAB_MAIN_DISP_H; ++y)
    {
        u8_t const c_bg = 0U;
        u16_t x = 0U;
        for (; x < CAB_MAIN_DISP_W / 2U - (gap / 2U); ++x)
        {
            FB_DRAW_PIX(FB_BUF_A, CAB_MAIN_DISP_X + x, CAB_MAIN_DISP_Y + y,
                        c_bg, c_bg, c_bg);
        }
        x += gap;
        for (; x < CAB_MAIN_DISP_W; ++x)
        {
            FB_DRAW_PIX(FB_BUF_A, CAB_MAIN_DISP_X + x, CAB_MAIN_DISP_Y + y,
                        c_bg, c_bg, c_bg);
        }
    }
    u8_t const margin = 8U;
    char main_freq_text[] = "MAIN    000000 HZ\0";
    char carr_freq_text[] = "CARRIER 000000 HZ\0";
    text_dec(vvvf_lut.freq_main, 6U, &main_freq_text[8U]);
    text_dec(vvvf_lut.freq_carrier, 6U, &carr_freq_text[8U]);
    text_draw(CAB_MAIN_DISP_X + margin, CAB_MAIN_DISP_Y + margin, 2U,
              main_freq_text, UINT8_MAX, UINT8_MAX, UINT8_MAX, 1U);
    text_draw(CAB_MAIN_DISP_X + (CAB_MAIN_DISP_W / 2U) + margin + (gap / 2U),
              CAB_MAIN_DISP_Y + margin, 2U, carr_freq_text, UINT8_MAX,
              UINT8_MAX, UINT8_MAX, 1U);
}
