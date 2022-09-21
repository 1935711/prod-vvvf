#pragma once

#include "rp.h"

#define DMA_OFF_CS 0x00
#define DMA_OFF_CONBLKAD 0x04
#define DMA_OFF_TI 0x08
#define DMA_OFF_SRCAD 0x0C
#define DMA_OFF_DSTAD 0x10
#define DMA_OFF_TXFRLEN 0x14
#define DMA_OFF_STRIDE 0x18
#define DMA_OFF_NEXTCONBK 0x1C
#define DMA_OFF_DEBUG 0x20

#define DMA_OFF_GLOBAL_INTSTA 0x0FE0
#define DMA_OFF_GLOBAL_ENABLE 0xFF0

typedef union dma_cs_u {
    struct
    {
        u8_t active : 1;   /* RW. Activate DMA. */
        u8_t end : 1;      /* W1C. DMA end flag. */
        u8_t intsta : 1;   /* W1C. Interrupt status. */
        u8_t dreq : 1;     /* RO. DREQ state. */
        u8_t paused : 1;   /* RO. DMA paused state. */
        u8_t dreqsdma : 1; /* RO. DMA paused by DREQ state. */
        u8_t wowl : 1; /* RO. DMA is waiting for last write to be received. */
        u8_t reserved0 : 1;      /**/
        u8_t error : 1;          /* RO. DMA error. */
        u8_t reserved1 : 7;      /**/
        u8_t priority : 4;       /* RW. AXI priority level. */
        u8_t panic_priority : 4; /* RW. AXI panic priority level. */
        u8_t reserved2 : 4;      /**/
        u8_t wow : 1;            /* RW. Wait for outstanding writes. */
        u8_t disdebug : 1;       /* RW. Disable debug pause signal. */
        u8_t abort : 1;          /* W1SC. Abort DMA. */
        u8_t reset : 1;          /* W1SC. DMA channel reset. */
    } __attribute__((packed)) f;
    u32_t v;
} dma_cs_ut;

typedef union dma_ti_u {
    struct
    {
        u8_t inten : 1;          /* RW. Interrupt enable. */
        u8_t tdmode : 1;         /* RW. 2D mode. Channels 0-6,15. */
        u8_t reserved0 : 1;      /**/
        u8_t wait_rsp : 1;       /* RW. Wait for a write response. */
        u8_t dst_inc : 1;        /* RW. Destination address increment. */
        u8_t dst_width : 1;      /* RW. Destination transfer width. */
        u8_t dst_dreq : 1;       /* RW. Control destination writes with DREQ. */
        u8_t dst_ignore : 1;     /* RW. Ignore writes. */
        u8_t src_inc : 1;        /* RW. Source address increment. */
        u8_t src_width : 1;      /* RW. Source transfer width. */
        u8_t src_dreq : 1;       /* RW. Control source reads with DREQ. */
        u8_t src_ignore : 1;     /* RW. Ignore reads. */
        u8_t burst_length : 4;   /* RW. Burst transfer length. */
        u8_t permap : 5;         /* RW. Peripheral mapping. */
        u8_t waits : 5;          /* RW. Add wait cycles. */
        u8_t no_wide_bursts : 1; /* RW. Don't do wide writes as 2 beat burst. */
    } __attribute__((packed)) f;
    u32_t v;
} dma_ti_ut;

typedef union dma_txfrlen_u {
    struct
    {
        u32_t len : 32; /* RW. Transfer length in bytes. */
    } __attribute__((packed)) f;
    struct
    {
        u16_t xlen : 16; /* RW. Transfer length in bytes. */
        u16_t ylen : 16; /* RW. Y transfer length when in 2D mode. Channels
                            0-6,15. */
    } __attribute__((packed)) f_lite;
    u32_t v;
} dma_txfrlen_ut;

typedef union dma_stride_u {
    struct
    {
        u16_t stride_dst : 16; /* RW. Source stride (2D mode). */
        u16_t stride_src : 16; /* RW. Destination stride (2D mode). */
    } __attribute__((packed)) f;
    u32_t v;
} dma_stride_ut;

typedef union dma_cbk_u {
    struct
    {
        dma_ti_ut ti;
        u32_t srcad;
        u32_t dstad;
        dma_txfrlen_ut txfrlen;
        dma_stride_ut stride;
        u32_t nextconbk;
        u32_t reserved0;
        u32_t reserved1;
    } __attribute__((packed)) f;
    u32_t v;
} dma_cbk_ut;
