#pragma once

#include "rp.h"

#define PWM_OFF_CTL 0x00
#define PWM_OFF_STA 0x04
#define PWM_OFF_DMAC 0x08
#define PWM_OFF_RNG1 0x10
#define PWM_OFF_DAT1 0x14
#define PWM_OFF_FIFO 0x18
#define PWM_OFF_RNG2 0x20
#define PWM_OFF_DAT2 0x24

#define PWM_FIFO_LEN 16U /* 16 x 32bit. */

typedef union pwm_ctl_u {
    struct
    {
        u8_t pwen1 : 1; /* RW. Ch 1 enable. */
        u8_t mode1 : 1; /* RW. Ch 1 mode. */
        u8_t rptl1 : 1; /* RW. Ch 1 repeat last data. */
        u8_t sbit1 : 1; /* RW. Ch 1 silence bit. */
        u8_t pola1 : 1; /* RW. Ch 1 polarity. */
        u8_t usef1 : 1; /* RW. Ch 1 use FIFO. */
        u8_t clrf : 1;  /* RO. Clear FIFO. */
        u8_t msen1 : 1; /* RW. Ch 1 M/S enable. */

        u8_t pwen2 : 1;     /* RW. Ch 2 enable. */
        u8_t mode2 : 1;     /* RW. Ch 2 mode. */
        u8_t rptl2 : 1;     /* RW. Ch 2 repeat last data. */
        u8_t sbit2 : 1;     /* RW. Ch 2 silence bit. */
        u8_t pola2 : 1;     /* RW. Ch 2 polarity. */
        u8_t usef2 : 1;     /* RW. Ch 2 use FIFO. */
        u8_t reserved0 : 1; /**/
        u8_t msen2 : 1;     /* RW. Ch 2 M/S enable. */
        /* Reserved. */
    } __attribute__((packed)) f;
    u32_t v;
} pwm_ctl_ut;

typedef union pwm_sta_u {
    struct
    {
        u8_t full1 : 1; /* RW. */
        u8_t empt1 : 1; /* RW. */
        u8_t werr1 : 1; /* RW. */
        u8_t rerr1 : 1; /* RW. */
        u8_t gapo1 : 1; /* RW. */
        u8_t gapo2 : 1; /* RW. */
        u8_t gapo3 : 1; /* RW. */
        u8_t gapo4 : 1; /* RW. */
        u8_t berr : 1;  /* RW. */
        u8_t sta1 : 1;  /* RW. */
        u8_t sta2 : 1;  /* RW. */
        u8_t sta3 : 1;  /* RW. */
        u8_t sta4 : 1;  /* RW. */
        /* Reserved. */

    } __attribute__((packed)) f;
    u32_t v;
} pwm_sta_ut;

typedef union pwm_dmac_u {
    struct
    {
        u8_t dreq : 8;        /* RW. DMA threshold for DREQ signal. */
        u8_t panic : 8;       /* RW. DMA threshold for PANIC signal. */
        u16_t reserved0 : 15; /**/
        u8_t enab : 1;        /* RW. DMA enable. */
    } __attribute__((packed)) f;
    u32_t v;
} pwm_dmac_ut;
