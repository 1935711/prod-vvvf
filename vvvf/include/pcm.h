#pragma once

#include "rp.h"

#define PCM_OFF_CS_A 0x00
#define PCM_OFF_FIFO_A 0x04
#define PCM_OFF_MODE_A 0x08
#define PCM_OFF_RXC_A 0x0C
#define PCM_OFF_TXC_A 0x10
#define PCM_OFF_DREQ_A 0x14
#define PCM_OFF_INTEN_A 0x18
#define PCM_OFF_INTSTC_A 0x1C
#define PCM_OFF_GRAY 0x20

#define PCM_FIFO_LEN 64U /* 64 x 32bit. */

typedef union pcm_cs_u {
    struct
    {
        u8_t en : 1;    /* RW. Enable PCM audio interface. */
        u8_t rxon : 1;  /* RW. Enable reception. */
        u8_t txon : 1;  /* RW. Enable transmission. */
        u8_t txclr : 1; /* WO. Clear TX FIFO. */
        u8_t rxclr : 1; /* WO. Clear RX FIFO. */
        u8_t txthr : 2; /* RW. Set TX FIFO threshold (when to set TXW flag). */
        u8_t rxthr : 2; /* RW. Set RX FIFO threshold (when to set RXW flag). */
        u8_t dmaen : 1; /* RW. DMA DREQ enable. */
        u8_t reserved0 : 3; /**/
        u8_t txsync : 1;    /* RO. TX FIFO sync. */
        u8_t rxsync : 1;    /* RO. RX FIFO sync. */
        u8_t txerr : 1;     /* RW. TX FIFO error. */
        u8_t rxerr : 1;     /* RW. RX FIFO error. */
        u8_t txw : 1;       /* RO. If TX FIFO needs writing. */
        u8_t rxr : 1;       /* RO. If RX FIFO needs reading. */
        u8_t txd : 1;       /* RO. If TX FIFO can accept data. */
        u8_t rxd : 1;       /* RO. If RX FIFO contains data. */
        u8_t txe : 1;       /* RO. TX FIFO is empty. */
        u8_t rxf : 1;       /* RO. RX FIFO is full. */
        u8_t rxsex : 1;     /* RW. RX sign extend. */
        u8_t sync : 1;      /* RW. PCM clock sync helper. */
        u8_t stby : 1;      /* RW. RAM standby. */
        u8_t reserved1 : 6; /**/
    } __attribute__((packed)) f;
    u32_t v;
} pcm_cs_ut;

void pcm_init(void);
void pcm_start_cpu(u8_t const *const audio, u32_t const audio_len);
void pcm_start_dma(u8_t const *const audio, u32_t const audio_len);
