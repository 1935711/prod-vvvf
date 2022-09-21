#include "vvvf.h"
#include "pbar.h"
#include <math.h>

void vvvf_calc(void)
{
    float const freq_main = 50.0F;
    float const freq_carrier = 1000.0F;

    volatile u32_t *const audio = (u32_t *)AUDIO_WAVE_ADDR;
    volatile vvvf_lut_item_st *const vvvf_lut =
        (vvvf_lut_item_st *)VVVF_LUT_ADDR;
    float freq_sin = freq_main;
    float freq_tri = freq_carrier;
    float freq_delta = 2.0F;

    for (u32_t sample_idx = 0U; sample_idx < VVVF_WAVE_LEN; sample_idx += 1U)
    {
        if (sample_idx % (AUDIO_WAVE_HZ * 2U) == 0U)
        {
            pbar_delta(1U, 'V');
        }

        vvvf_lut_item_st const vvvf_lut_item = {
            .freq_main = floor(freq_sin),
            .freq_carrier = floor(freq_tri),
            .freq_main_delta = floor(freq_delta),
        };
        vvvf_lut[sample_idx] = vvvf_lut_item;

        float const sample_sin =
            sin((M_PI * 2.0F) * freq_sin *
                ((float)sample_idx / (float)AUDIO_WAVE_HZ)) +
            1.0F;
        float const sample_tri =
            ((2.0F / M_PI) *
             asin(sin(((M_PI * 2.0F) / freq_tri) * sample_idx))) +
            1.0F;

        /* For stereo, write the sample sample to both channels. */
        u8_t const pwm_sin = sample_sin > sample_tri ? VVVF_WAVE_A : 0U;

        /* Write acceleration to start of audio. */
        audio[sample_idx * AUDIO_WAVE_CH] = pwm_sin;
        audio[sample_idx * AUDIO_WAVE_CH + 1U] = pwm_sin;

        /* Write deceleration to end of audio. */
        audio[(AUDIO_WAVE_LEN * AUDIO_WAVE_CH) - (sample_idx * AUDIO_WAVE_CH) -
              1U] = pwm_sin;
        audio[(AUDIO_WAVE_LEN * AUDIO_WAVE_CH) -
              (sample_idx * AUDIO_WAVE_CH + 1U) - 1U] = pwm_sin;

        if (sample_idx == 0U)
        {
            freq_sin = freq_main;
            freq_tri = freq_carrier;
            freq_delta = 10.0F;
        }
        else if (sample_idx == 2U * (AUDIO_WAVE_HZ / 4U))
        {
            freq_sin += 8.0F;
        }
        else if (sample_idx == 4U * (AUDIO_WAVE_HZ / 4U))
        {
            freq_sin += 8.0F;
        }
        else if (sample_idx == AUDIO_WAVE_HZ * 2U)
        {
            freq_tri = 6000.0F;
            freq_delta = 17.0F;
        }
        else if (sample_idx == AUDIO_WAVE_HZ * 4U)
        {
            freq_tri = 4000.0F;
            freq_delta = 10.0F;
        }
        else if (sample_idx == AUDIO_WAVE_HZ * 6U)
        {
            freq_tri = 2000.0F;
            freq_delta = 8.0F;
        }
        else if (sample_idx == AUDIO_WAVE_HZ * 10U)
        {
            freq_tri = 500.0F;
            freq_delta = 2.0F;
        }
        else if (sample_idx == AUDIO_WAVE_HZ * 16U)
        {
            freq_tri = 250.0F;
            freq_delta = 2.0F;
        }
        else if (sample_idx == AUDIO_WAVE_HZ * 20U)
        {
            freq_delta = 0.01F;
        }

        if (freq_sin < 300.0F && freq_delta > 0.001F)
        {
            freq_sin += freq_delta / AUDIO_WAVE_HZ;
        }
    }

    audio_fade(&audio[(VVVF_WAVE_LEN * AUDIO_WAVE_CH) - (2U * AUDIO_WAVE_HZ)],
               AUDIO_WAVE_HZ * 2U, 1U);
    audio_fade(&audio[(AUDIO_WAVE_LEN * AUDIO_WAVE_CH) -
                      (VVVF_WAVE_LEN * AUDIO_WAVE_CH)],
               AUDIO_WAVE_HZ * 2U, 0U);
}
