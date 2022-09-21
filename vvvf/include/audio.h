#pragma once
#include "dma.h"
#include "rp.h"

extern u8_t const _binary_data_audio_bin_start;
extern u8_t const _binary_data_audio_bin_end;

#define AUDIO_SONG_DUR 106U /* Seconds. */

#define AUDIO_ADDR 0x00400000
#define AUDIO_DMA_CH 0U

#define AUDIO_WAVE_ADDR AUDIO_ADDR
#define AUDIO_WAVE_D 4U
#define AUDIO_WAVE_DUR ((24U * 2U) + AUDIO_SONG_DUR) /* Seconds. */
#define AUDIO_WAVE_LEN (AUDIO_WAVE_HZ * AUDIO_WAVE_DUR)
#define AUDIO_WAVE_CH 2U /* Stereo. */
#define AUDIO_WAVE_SIZ (AUDIO_WAVE_LEN * AUDIO_WAVE_D * AUDIO_WAVE_CH)
#define AUDIO_WAVE_SONG_SIZ                                                    \
    (AUDIO_SONG_DUR * AUDIO_WAVE_HZ * AUDIO_WAVE_CH * AUDIO_WAVE_D)
#define AUDIO_WAVE_HZ 20000U

#define AUDIO_CBK_ADDR (AUDIO_WAVE_ADDR + AUDIO_WAVE_SIZ)
#define AUDIO_CBK_D (sizeof(dma_cbk_ut))
#define AUDIO_CBK_LEN_XTRA 2U
/**
 * How many control blocks are needed to stream whole wave with DMA (each
 * transfer can do UINT32_MAX bytes) plus AUDIO_CBK_LEN_XTRA extra blocks for a
 * looping sound at the end.
 */
#define AUDIO_CBK_LEN (1U + AUDIO_CBK_LEN_XTRA)
#define AUDIO_CBK_SIZ (AUDIO_CBK_LEN * AUDIO_CBK_D)

_Static_assert(AUDIO_CBK_ADDR % 32U == 0U,
               "Audio control blocks are not aligned to 256-bit boundaries.");

#define AUDIO_SIZ (AUDIO_WAVE_SIZ + AUDIO_CBK_SIZ)

void audio_init(void);
void audio_start_cpu(void);
void audio_start_dma(void);

void audio_fade(volatile u32_t *const start, u32_t const len, u8_t const fw_bk);
