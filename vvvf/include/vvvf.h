#pragma once

#include "audio.h"
#include "tunnel.h"

typedef struct vvvf_lut_item_s
{
    u16_t freq_main;
    u16_t freq_carrier;
    u16_t freq_main_delta;
    u16_t reserved;
} __attribute__((packed)) vvvf_lut_item_st;

#define VVVF_WAVE_DUR (24U)
#define VVVF_WAVE_LEN (VVVF_WAVE_DUR * AUDIO_WAVE_HZ)
#define VVVF_WAVE_SIZ (VVVF_WAVE_LEN * AUDIO_WAVE_CH * sizeof(u32_t))
#define VVVF_WAVE_A (128U)

#define VVVF_ADDR (TUNNEL_ADDR + TUNNEL_SIZ + 0x01000000)

#define VVVF_LUT_ADDR VVVF_ADDR
#define VVVF_LUT_LEN (VVVF_WAVE_LEN)
#define VVVF_LUT_D (sizeof(vvvf_lut_item_st))
#define VVVF_LUT_SIZ (VVVF_LUT_D * VVVF_LUT_LEN)

#define VVVF_SIZ (VVVF_LUT_SIZ)

void vvvf_calc(void);
