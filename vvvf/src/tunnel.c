#include "tunnel.h"
#include "pbar.h"
#include <math.h>

volatile u8_t (*tex)[TUNNEL_TEX_H][TUNNEL_TEX_W][TUNNEL_TEX_D] =
    (volatile u8_t(*)[TUNNEL_TEX_H][TUNNEL_TEX_W][TUNNEL_TEX_D])TUNNEL_TEX_ADDR;
volatile i16_t (*const lut_dph)[TUNNEL_LUT_H][TUNNEL_LUT_W] =
    (volatile i16_t(*)[TUNNEL_LUT_H][TUNNEL_LUT_W])TUNNEL_DPH_ADDR;
volatile i16_t (*const lut_ang)[TUNNEL_LUT_H][TUNNEL_LUT_W] =
    (volatile i16_t(*)[TUNNEL_LUT_H][TUNNEL_LUT_W])TUNNEL_ANG_ADDR;
volatile u8_t (*const lut_shd)[TUNNEL_LUT_H][TUNNEL_LUT_W] =
    (volatile u8_t(*)[TUNNEL_LUT_H][TUNNEL_LUT_W])TUNNEL_SHD_ADDR;

void tunnel_calc_tex_audio(void)
{
    for (u16_t y = 0; y < TUNNEL_TEX_H; ++y)
    {
        for (u16_t x = 0; x < TUNNEL_TEX_W; ++x)
        {
            u32_t const *const audio = (u32_t *)AUDIO_WAVE_ADDR;
            (*tex)[y][x][0U] = audio[(y * 1U) * TUNNEL_TEX_W + x * 1U];
            (*tex)[y][x][1U] = audio[(y * 2U) * TUNNEL_TEX_W + x * 2U];
            (*tex)[y][x][2U] = audio[(y * 3U) * TUNNEL_TEX_W + x * 3U];
        }
    }
}

void tunnel_calc_tex_subway(void)
{
    for (u16_t y = 0; y < TUNNEL_TEX_H; ++y)
    {
        for (u16_t x = 0; x < TUNNEL_TEX_W; ++x)
        {
            i32_t const dst_center_y_s = y - (TUNNEL_TEX_H / 2U);
            u16_t const dst_center_y =
                dst_center_y_s < 0 ? -dst_center_y_s : dst_center_y_s;

            i32_t const dst_center_x_s = x - (TUNNEL_TEX_W / 2U);
            u16_t const dst_center_x =
                dst_center_x_s < 0 ? -dst_center_x_s : dst_center_x_s;

            u8_t const center = dst_center_y == 0U ? 1U : 0U;
            u8_t const rail_inside = dst_center_y == 15U ? 1U : 0U;
            u8_t const rail = dst_center_y == 16U ? 1U : 0U;

            u8_t const light_spread = 8U;
            u8_t const light = dst_center_x <= light_spread ? 1U : 0U;
            i8_t const light_dst_s =
                ((i16_t)(dst_center_x % 32U) - (i16_t)light_spread);
            u8_t const light_dst = light_dst_s < 0 ? -light_dst_s : light_dst_s;
            u8_t const light_c = light_dst * (0xD0 / light_spread);

            u8_t const r =
                (0x2E * center) | (0x7B * rail) | (0x1C * rail_inside);
            u8_t const g =
                (0x2B * center) | (0x75 * rail) | (0x17 * rail_inside);
            u8_t const b =
                (0x30 * center) | (0x71 * rail) | (0x18 * rail_inside);

            (*tex)[y][x][0U] =
                r + (light_c * light) > 255 ? 255 : r + (light_c * light);
            (*tex)[y][x][1U] =
                g + (light_c * light) > 255 ? 255 : g + (light_c * light);
            (*tex)[y][x][2U] =
                b + (light_c * light) > 255 ? 255 : b + (light_c * light);
        }
    }

    /* Add some random stars. */
    xorsh32_st xor_state = {.a = 1935711U};
    for (u16_t r_i = 0U; r_i < 100U; ++r_i)
    {
        u32_t const rand = xorshift32(&xor_state);
        u16_t const x = rand % TUNNEL_TEX_W;
        u16_t const y = (rand % (TUNNEL_TEX_W * TUNNEL_TEX_H)) / TUNNEL_TEX_H;
        u8_t const c = 255U;
        (*tex)[y][x][0U] = c;
        (*tex)[y][x][1U] = c;
        (*tex)[y][x][2U] = c;
    }
}

void tunnel_calc_lut(void)
{
    for (u16_t y = 0; y < TUNNEL_LUT_H; ++y)
    {
        u32_t const ypow2 = (y - TUNNEL_H) * (y - TUNNEL_H);
        for (u16_t x = 0; x < TUNNEL_LUT_W; ++x)
        {
            u32_t const xpow2 = (x - TUNNEL_W) * (x - TUNNEL_W);
            u32_t const center_dst = sqrt(xpow2 + ypow2);
            i16_t const depth =
                (i32_t)floor(10.0F * (float)TUNNEL_TEX_H / center_dst) %
                TUNNEL_TEX_H;
            i16_t const angle =
                round(0.5F * (float)TUNNEL_TEX_W *
                      (atan2(y - (i32_t)TUNNEL_H, x - (i32_t)TUNNEL_W) +
                       (M_PI / 2.0F)) /
                      M_PI);

            (*lut_dph)[y][x] = depth;
            (*lut_ang)[y][x] = angle;
            (*lut_shd)[y][x] = 255U - (center_dst > 255U ? 255U : center_dst);
        }

        if (y % (TUNNEL_LUT_H / 6U) == 0U)
        {
            pbar_delta(1U, 'T');
        }
    }
}

void tunnel_draw(float const anim)
{
#if TUNNEL_ANIM_X_SHIFT_TOGGLE == 1
    i16_t const shift_x = TUNNEL_TEX_W * 1.00F * anim;
#else
    i16_t const shift_x = 0;
#endif
#if TUNNEL_ANIM_Y_SHIFT_TOGGLE == 1
    i16_t const shift_y = TUNNEL_TEX_H * 0.25F * (anim / 32.0F);
#else
    i16_t const shift_y = 0;
#endif
#if TUNNEL_ANIM_X_LOOK_TOGGLE == 1
    i16_t const center_x =
        ((i16_t)TUNNEL_W / 2) +
        (i16_t)floor((TUNNEL_W / 8U) * (sin(fabs(anim) / 4.0F) * 0.2F));
#else
    i16_t const center_x = (i16_t)TUNNEL_W / 2;
#endif
#if TUNNEL_ANIM_Y_LOOK_TOGGLE == 1
    i16_t const center_y =
        ((i16_t)TUNNEL_H / 2U) +
        (i16_t)floor((TUNNEL_H / 8U) *
                     sin((fabs(anim) > 2.0F ? 2.0F : fabs(anim)) * 2.0F));
#else
    i16_t const center_y = 0;
#endif

    for (u16_t y = 0; y < TUNNEL_H; ++y)
    {
        for (u16_t x = 0; x < TUNNEL_W; ++x)
        {
            u16_t const u = (*lut_dph)[y + center_y][x + center_x] + shift_x;
            u16_t const v = (*lut_ang)[y + center_y][x + center_x] + shift_y;
            u8_t const shd = (*lut_shd)[y + center_y][x + center_x];

            volatile u8_t const(*const c)[TUNNEL_TEX_D] =
                &(*tex)[v % TUNNEL_TEX_H][u % TUNNEL_TEX_W];
            u8_t const r = ((*c)[0U] * shd) / UINT8_MAX;
            u8_t const g = ((*c)[1U] * shd) / UINT8_MAX;
            u8_t const b = ((*c)[2U] * shd) / UINT8_MAX;
            FB_DRAW_PIX(FB_BUF_A, x, y, r, g, b);
        }
    }
}

void tunnel_draw_dph(void)
{
    for (u16_t y = 0; y < TUNNEL_H; ++y)
    {
        for (u16_t x = 0; x < TUNNEL_W; ++x)
        {
            u8_t const c_raw = (*lut_dph)[y * 2U][x * 2U];
            u8_t const c = (c_raw * (i32_t)UINT8_MAX) / (i32_t)INT16_MAX;
            FB_DRAW_PIX(FB_BUF_A, x, y, c_raw, c, c);
        }
    }
}

void tunnel_draw_ang(void)
{
    for (u16_t y = 0; y < TUNNEL_H; ++y)
    {
        for (u16_t x = 0; x < TUNNEL_W; ++x)
        {
            u8_t const c_raw = (*lut_ang)[y * 2U][x * 2U];
            u8_t const c = (c_raw * (u32_t)UINT8_MAX) / (u32_t)UINT16_MAX;
            FB_DRAW_PIX(FB_BUF_A, x, y, c_raw, c, c);
        }
    }
}

void tunnel_draw_shd(void)
{
    for (u16_t y = 0; y < TUNNEL_H; ++y)
    {
        for (u16_t x = 0; x < TUNNEL_W; ++x)
        {
            u8_t const c = (*lut_shd)[y * 2U][x * 2U];
            FB_DRAW_PIX(FB_BUF_A, x, y, c, c, c);
        }
    }
}

void tunnel_draw_tex(void)
{
    for (u16_t y = 0; y < TUNNEL_H; ++y)
    {
        for (u16_t x = 0; x < TUNNEL_W; ++x)
        {
            volatile u8_t(*c)[TUNNEL_TEX_D] =
                &(*tex)[y % TUNNEL_TEX_H][x % TUNNEL_TEX_W];
            FB_DRAW_PIX(FB_BUF_A, x, y, (*c)[0U], (*c)[1U], (*c)[2U]);
        }
    }
}
