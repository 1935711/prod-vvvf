#include "pcm.h"
#include "gpio.h"
#include "pwm.h"
#include "uart0.h"

void pcm_init(void)
{
}

void pcm_start_cpu(u8_t const *const audio, u32_t const audio_len)
{
    volatile u32_t *const pcm_cs_a = RP_P_ADDR(RP_P_PCM_BASE, PCM_OFF_CS_A);
    volatile u32_t *const pcm_fifo_a = RP_P_ADDR(RP_P_PCM_BASE, PCM_OFF_FIFO_A);

    pcm_cs_ut cs_val = {0U};

    /* Make sure it is disabled. */
    *pcm_cs_a = cs_val.v;
    cs_val.f.en = 1U;
    *pcm_cs_a = cs_val.v;

    cs_val.f.txclr = 1U;
    cs_val.f.rxclr = 1U;
    *pcm_cs_a = cs_val.v;
    /* TODO: Wait for 2 PCM clocks. */
    delay(0x10000);

    /* THR flag will be set when FIFO is empty. */
    cs_val.f.txthr = 0U;
    *pcm_cs_a = cs_val.v;

    /* Fill FIFO with data before enabling transmission. */
    u32_t audio_idx = 0U;
    for (u8_t data_idx = 0; data_idx < PCM_FIFO_LEN; ++data_idx, ++audio_idx)
    {
        *pcm_fifo_a = audio[audio_idx];
        audio_idx %= audio_len;
    }

    /* Enable transmission. */
    cs_val.f.txon = 1U;
    *pcm_cs_a = cs_val.v;

    pcm_cs_ut cs_cpy;
    for (;;)
    {
        do
        {
            cs_cpy.v = *pcm_cs_a;
        } while (~cs_cpy.f.txw);
        *pcm_fifo_a = audio[audio_idx++];
        audio_idx %= audio_len;
    }
}

void pcm_start_dma(u8_t const *const audio, u32_t const audio_len)
{
}
