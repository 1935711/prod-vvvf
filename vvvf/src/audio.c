#include "audio.h"
#include "cm.h"
#include "dma.h"
#include "gpio.h"
#include "pbar.h"
#include "pwm.h"
#include "vvvf.h"

volatile dma_cbk_ut (*const audio_dma_cbk)[AUDIO_CBK_LEN] =
    (dma_cbk_ut(*)[AUDIO_CBK_LEN])AUDIO_CBK_ADDR;

void audio_init(void)
{
    /* Configure pins of the 3.5mm jack for PWM0 and PWM1. */
    u64_t const pin_mask = (u64_t)1 << (u64_t)GPIO_PIN_PWM0_JACK |
                           (u64_t)1 << (u64_t)GPIO_PIN_PWM1_JACK;
    gpio_cfg_pull(pin_mask, GPIO_PULL_OFF);
    gpio_cfg_func(pin_mask, GPIO_FUNC_0);

    /* Setup PWM clock. */
    volatile u32_t *const cm_pwm_ctl = RP_P_ADDR(RP_P_CM_BASE, CM_OFF_PWM_CTL);
    volatile u32_t *const cm_pwm_div = RP_P_ADDR(RP_P_CM_BASE, CM_OFF_PWM_DIV);

    {
        /* Stop the PWM clock. */
        cm_ctl_ut cm_ctl_val = {
            .f =
                {
                    .kill = 1U,
                    .pwd = CM_PWD,
                },
        };
        *cm_pwm_ctl = cm_ctl_val.v;
        do
        {
            cm_ctl_val.v = *cm_pwm_ctl;
        } while (cm_ctl_val.f.busy);
    }
    {
        cm_div_ut cm_div_val = {
            .f =
                {
                    .divf = 0U,
                    .divi = 3U,
                    .pwd = CM_PWD,
                },
        };
        *cm_pwm_div = cm_div_val.v;
    }
    {
        cm_ctl_ut cm_ctl_val = {
            .f =
                {
                    .src = 1U, /* XOSC (19.2MHz). */
                    .enab = 1U,
                    .pwd = CM_PWD,
                },
        };
        *cm_pwm_ctl = cm_ctl_val.v;
        do
        {
            cm_ctl_val.v = *cm_pwm_ctl;
        } while (cm_ctl_val.f.busy);
    }

    volatile u32_t *const pwm_ctl = RP_P_ADDR(RP_P_PWM_BASE, PWM_OFF_CTL);
    volatile u32_t *const pwm_rng1 = RP_P_ADDR(RP_P_PWM_BASE, PWM_OFF_RNG1);
    volatile u32_t *const pwm_rng2 = RP_P_ADDR(RP_P_PWM_BASE, PWM_OFF_RNG2);
    volatile u32_t *const pwm_dmac = RP_P_ADDR(RP_P_PWM_BASE, PWM_OFF_DMAC);
    volatile u32_t *const pwm_sta = RP_P_ADDR(RP_P_PWM_BASE, PWM_OFF_STA);

    /* Clear all errors. */
    pwm_sta_ut const sta_err_clr = {
        .f =
            {
                .gapo1 = 1U,
                .gapo2 = 1U,
                .rerr1 = 1U,
                .werr1 = 1U,
            },
    };

    /**
     * Sample rate = Clock / Divider / Range
     *             = cm_pwm_src / div_i,div_f / pwm_rng.
     *
     *       20KHz = 19.2MHz / 3 / 320
     */

    /* Reset PWM. */
    *pwm_ctl = 0U;
    delay(0x20000);
    *pwm_sta = sta_err_clr.v;
    delay(0x20000);

    *pwm_rng1 = 0x140;
    *pwm_rng2 = 0x140;

    pwm_dmac_ut const dma_dmac_val = {
        .f =
            {
                .dreq = 15U,
                .panic = 15U,
                .enab = 1U,
            },
    };
    *pwm_dmac = dma_dmac_val.v;
    delay(0x20000);

    pwm_ctl_ut const pwm_ctl_clrf = {.f.clrf = 1U};
    *pwm_ctl |= pwm_ctl_clrf.v;
    delay(0x20000);

    pwm_ctl_ut const pwm_ctl_val = {
        .f =
            {
                .pwen1 = 1U,
                .usef1 = 1U,
                .pwen2 = 1U,
                .usef2 = 1U,
            },
    };
    *pwm_ctl = pwm_ctl_val.v;

    /* Copy the song to the right place in memory. */
    volatile u32_t *const audio = (u32_t *)(AUDIO_WAVE_ADDR + VVVF_WAVE_SIZ);
    volatile u8_t const *const song = &_binary_data_audio_bin_start;
    u32_t const song_len = AUDIO_SONG_DUR * AUDIO_WAVE_HZ;
    for (u32_t sample_idx = 0U; sample_idx < song_len; ++sample_idx)
    {
        audio[sample_idx * 2U] = song[sample_idx];
        audio[sample_idx * 2U + 1U] = song[sample_idx];

        if (sample_idx % (song_len / 10U) == 0U)
        {
            pbar_delta(1U, 'A');
        }
    }
    audio_fade(&audio[((VVVF_WAVE_DUR + AUDIO_SONG_DUR) * AUDIO_WAVE_HZ *
                       AUDIO_WAVE_CH) -
                      (AUDIO_WAVE_HZ * 2U)],
               AUDIO_WAVE_HZ * 2U, 1U);
}

void audio_start_cpu(void)
{
    volatile u32_t *const pwm_sta = RP_P_ADDR(RP_P_PWM_BASE, PWM_OFF_STA);
    volatile u32_t *const pwm_fifo = RP_P_ADDR(RP_P_PWM_BASE, PWM_OFF_FIFO);

    u32_t const *const audio = (u32_t *)AUDIO_WAVE_ADDR;

    pwm_sta_ut const sta_err_clr = {
        .f =
            {
                .gapo1 = 1U,
                .gapo2 = 1U,
                .rerr1 = 1U,
                .werr1 = 1U,
            },
    };

    u32_t sample_idx = 0U;
    pwm_sta_ut sta_val;
    for (;;)
    {
        sta_val.v = *pwm_sta;
        if (!sta_val.f.full1)
        {
            do
            {
                *pwm_fifo = audio[sample_idx++];
                *pwm_fifo = audio[sample_idx++];
                sta_val.v = *pwm_sta;
                if (sample_idx >= AUDIO_WAVE_LEN * AUDIO_WAVE_CH)
                {
                    return;
                }
            } while (!sta_val.f.full1);
        }
        /* Clear errors. */
        if (sta_val.v & sta_err_clr.v)
        {
            *pwm_sta = sta_err_clr.v;
        }
    }
}

void audio_start_dma(void)
{
    volatile u32_t *const dma_cs =
        RP_P_ADDR(RP_P_DMA_BASE(AUDIO_DMA_CH), DMA_OFF_CS);
    volatile u32_t *const dma_conblkad =
        RP_P_ADDR(RP_P_DMA_BASE(AUDIO_DMA_CH), DMA_OFF_CONBLKAD);

    dma_cs_ut const dma_cs_reset = {.f.reset = 1U};
    dma_cs_ut const dma_cs_abort = {.f.abort = 1U};
    dma_cs_ut const dma_cs_flag = {.f = {.end = 1U, .intsta = 1U}};
    dma_cs_ut const dma_cs_cfg = {
        .f =
            {
                .disdebug = 1U,
                .wow = 1U,

                .priority = 8U,
                .panic_priority = 8U,
            },
    };
    dma_cs_ut const dma_cs_active = {.f = {.active = 1U}};

    (*audio_dma_cbk)[0U] = (dma_cbk_ut){
        .f =
            {
                .ti =
                    {
                        .f =
                            {
                                .inten = 0U,
                                .tdmode = 0U,
                                .wait_rsp = 1U,
                                .dst_inc = 0U,
                                .dst_width = 0U,
                                .dst_dreq = 1U,
                                .dst_ignore = 0U,
                                .src_inc = 1U,
                                .src_width = 0U,
                                .src_dreq = 0U,
                                .src_ignore = 0U,
                                .burst_length = 0U,
                                .permap = 5U,
                                .waits = 0U,
                                .no_wide_bursts = 1U,
                            },
                    },
                .srcad = AUDIO_WAVE_ADDR,
                .dstad =
                    RP_P_BUS_ADDR((u32_t)RP_P_ADDR(RP_P_PWM_BASE, PWM_OFF_FIFO),
                                  RP_P_BUS_ARM),
                .txfrlen =
                    {
                        .f =
                            {

                                .len = AUDIO_WAVE_SIZ,
                            },
                    },
                .stride =
                    {
                        .f =
                            {
                                .stride_dst = 0U,
                                .stride_src = 0U,
                            },
                    },
                .nextconbk = 0U,
                .reserved0 = 0U,
                .reserved1 = 0U,
            },
    };

    *dma_cs = dma_cs_abort.v;
    *dma_cs = dma_cs_reset.v;
    *dma_cs = dma_cs_flag.v;
    *dma_conblkad = (u32_t)(&(*audio_dma_cbk)[0U]);
    *dma_cs = dma_cs_cfg.v;
    *dma_cs |= dma_cs_active.v;
}

void audio_fade(volatile u32_t *const start, u32_t const len, u8_t const fw_bk)
{
    /**
     * FW:   0 <- 255 -1
     * BK: 255 -> 0   -1
     */
    u8_t vol_delta = UINT8_MAX;
    i8_t const vol_step = -1;

    for (u32_t sample_i = 0U; sample_i < len; sample_i += 1U)
    {
        u32_t const sample_idx =
            fw_bk ? (len - (sample_i * 2)) - 2 : sample_i * 2U;
        u8_t const sample0 = start[sample_idx];
        u8_t const sample1 = start[sample_idx + 1U];

        start[sample_idx] = sample0 + vol_delta < 0 ? 0U
                            : sample0 + vol_delta > UINT8_MAX
                                ? UINT8_MAX
                                : (u8_t)(sample0 + vol_delta);
        start[sample_idx + 1U] = sample1 + vol_delta < 0 ? 0U
                                 : sample1 + vol_delta > UINT8_MAX
                                     ? UINT8_MAX
                                     : (u8_t)(sample1 + vol_delta);

        if (sample_idx % (AUDIO_WAVE_HZ / 200U) == 0U)
        {
            if (vol_delta + vol_step >= 0)
            {
                vol_delta += vol_step;
            }
        }
    }
}
