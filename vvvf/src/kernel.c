#include "audio.h"
#include "cab.h"
#include "fb.h"
#include "gpio.h"
#include "pbar.h"
#include "rp.h"
#include "shader.h"
#include "text.h"
#include "time.h"
#include "tunnel.h"
#include "uart0.h"
#include "vvvf.h"
#include <math.h>

#define DEBUG_FRAME 0

#define SAMPLE_IDX_SEC(sec) (sec * AUDIO_WAVE_HZ * AUDIO_WAVE_CH * AUDIO_WAVE_D)

static float const anim_delta = 0.01F;

void kernel_main(u32_t const zero, u32_t const machine_id,
                 u32_t const *const atags)
{
    /* Init. */
    {
        fb_init();
        pbar_delta(1U, 'F');

        gpio_cfg_pull(1U << GPIO_PIN_LED_ACT, GPIO_PULL_UP);
        gpio_cfg_func(1U << GPIO_PIN_LED_ACT, GPIO_FUNC_OUT);
        pbar_delta(1U, 'G');

        uart0_init();
        pbar_delta(1U, 'U');

        tunnel_calc_tex_subway();
        pbar_delta(4U, 'T');

        tunnel_calc_lut();
        /* Progress updated inside the func. */

        shader_raymarch_calc_frames(anim_delta);
        /* Progress updated inside the func. */

        shader_malina_calc_leaves();
        shader_malina_calc_frames(anim_delta * 2U);
        pbar_delta(4U, 'M');

        audio_init();
        vvvf_calc();
        pbar_delta(4U, 'A');

        pbar(100U, 'K');
        delay(0x19357110);

        fb_clr();
        cab_draw_main();
    }

    volatile u32_t *const dma_audio_txfrlen =
        RP_P_ADDR(RP_P_DMA_BASE(AUDIO_DMA_CH), DMA_OFF_TXFRLEN);

    u32_t shader_raymarch_frame = 0U;
    u32_t shader_malina_frame = 0U;
    i8_t shader_malina_frame_delta = 1;
    float tunnel_anim_accel = 0.0F;
    float tunnel_anim_decel = 0.0F;

    audio_start_dma();
    for (u32_t i = 0U;; ++i)
    {
        u32_t const sample_idx = AUDIO_WAVE_SIZ - *dma_audio_txfrlen;
        float const time =
            (float)sample_idx / AUDIO_WAVE_CH / AUDIO_WAVE_D / AUDIO_WAVE_HZ;

        if (sample_idx < SAMPLE_IDX_SEC(time_step[0U]))
        {
            static float anim_start;
            static u8_t once_0 = 1U;
            if (once_0)
            {
                once_0 = 0U;
                anim_start = 0.0F;
            }

            u32_t const vvvf_lut_idx = sample_idx / VVVF_LUT_D;
            vvvf_lut_item_st const vvvf_lut =
                (*((volatile vvvf_lut_item_st(*)[VVVF_LUT_LEN])
                       VVVF_LUT_ADDR))[vvvf_lut_idx];

            anim_start += ((vvvf_lut.freq_main - 50U) / 1500.0F);
            tunnel_anim_accel = anim_start;
            tunnel_draw(tunnel_anim_accel);

            cab_draw_overlay();
            cab_draw_disp(vvvf_lut_idx);

            static u8_t once_1 = 1U;
            if (sample_idx > (SAMPLE_IDX_SEC(time_step[0U]) + 20.0F) && once_1)
            {
                once_1 = 0U;
                tunnel_calc_tex_audio();
            }
        }
        else if (sample_idx < SAMPLE_IDX_SEC(time_step[1U]))
        {
            static u8_t once = 1U;
            if (once)
            {
                once = 0U;
                fb_clr();
            }
            shader_raymarch_draw(time, shader_raymarch_frame);
            shader_raymarch_frame += 1U;
            shader_raymarch_frame %= SHADER_RAYMARCH_FRAME_COUNT;
        }
        else if (sample_idx < SAMPLE_IDX_SEC(time_step[2U]))
        {
            static u8_t once = 1U;
            if (once)
            {
                once = 0U;
                fb_clr();
            }

            shader_field_draw(time * 2.4F);
        }
        else if (sample_idx < SAMPLE_IDX_SEC(time_step[3U]))
        {
            static u8_t once = 1U;
            if (once)
            {
                once = 0U;
                fb_clr();
            }

            shader_malina_draw(time, shader_malina_frame);

            shader_malina_frame += shader_malina_frame_delta;
            if (shader_malina_frame >= SHADER_MALINA_FRAME_COUNT - 1U)
            {
                shader_malina_frame_delta = -1;
            }
            else if (shader_malina_frame == 0U)
            {
                shader_malina_frame_delta = +1;
            }

            text_draw(20U, (FB_H - 40U) - (sin(time * 2.0F) * 10), 2U,
                      "10 YEARS OF RPI 1B!", 20U, 255U, 40U, 0U);
        }
        else if (sample_idx < SAMPLE_IDX_SEC(time_step[4U]))
        {
            static u8_t once = 1U;
            if (once)
            {
                once = 0U;
                fb_clr();
            }

            shader_wave_draw(time * 2.0F);
        }
        else if (sample_idx < SAMPLE_IDX_SEC(time_step[5U]))
        {
            static float anim_start;
            static u8_t once = 1U;
            if (once)
            {
                once = 0U;
                anim_start = tunnel_anim_accel;
                fb_clr();
                cab_draw_main();
            }

            u32_t const vvvf_lut_idx =
                (VVVF_LUT_LEN - 1U) -
                ((sample_idx - SAMPLE_IDX_SEC(time_step[4U])) / VVVF_LUT_D);

            vvvf_lut_item_st const vvvf_lut =
                (*((volatile vvvf_lut_item_st(*)[VVVF_LUT_LEN])
                       VVVF_LUT_ADDR))[vvvf_lut_idx];
            anim_start -= ((vvvf_lut.freq_main - 50U) / 1500.0F);
            tunnel_anim_decel = anim_start;
            tunnel_draw(-tunnel_anim_decel);

            cab_draw_overlay();
            cab_draw_disp(vvvf_lut_idx);
        }
        else
        {
            static u8_t once = 1U;
            if (once)
            {
                once = 0U;
                fb_clr();
            }
            text_draw((FB_W / 2U) - (3U * 3U * 11U), (FB_H / 2U) - 38U, 3U,
                      "VVVF     1935711\r\n"
                      "WILD XENIUM 2022\r\n\n"
                      "    THE  END    ",
                      UINT8_MAX, UINT8_MAX, UINT8_MAX, 0U);
        }

#if DEBUG_FRAME == 1
        if (i % 2 == 0U)
        {
            text_draw(FB_W - 24U, 12U, 4U, ".", 80U, 80U, 80U, 0U);
        }
        else
        {
            text_draw(FB_W - 24U, 12U, 4U, " ", 80U, 80U, 80U, 0U);
        }

        char time_text[] = "TIME 0000";
        text_dec((u32_t)time, 4U, &time_text[5U]);
        text_draw(10U, 10U, 2U, time_text, 80U, 80U, 80U, 0U);
#endif

        fb_buf_swap();
    }

    panic();
}
