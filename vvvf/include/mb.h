#pragma once

#include "rp.h"

#define MB_OFF_READ 0x00
#define MB_OFF_PEEK 0x10
#define MB_OFF_SENDER 0x14
#define MB_OFF_STATUS 0x18
#define MB_OFF_CONF 0x1C
#define MB_OFF_WRITE 0x20

typedef enum mb_ch_e
{
    MB_CH_MGMTPOWER = 0U,
    MB_CH_FRAMEBUF = 1U,
    MB_CH_UARTVIRT = 2U,
    MB_CH_VCHIQ = 3U,
    MB_CH_LED = 4U,
    MB_CH_BUTTON = 5U,
    MB_CH_TOUCH = 6U,
    MB_CH_7 = 7U,
    MB_CH_PROPOUT = 8U,
    MB_CH_PROPIN = 9U,
} mb_ch_et;

typedef union mb_status_u {
    struct
    {
        u32_t reserved : 30;
        u8_t empty : 1;
        u8_t full : 1;
    } __attribute__((packed)) f;
    u32_t v;
} mb_status_ut;

/* Read and write registers have the same format (both hold a message). */
typedef union mb_msg_u {
    struct
    {
        u8_t ch : 4;
        u32_t data : 28;
    } __attribute__((packed)) f;
    u32_t v;
} mb_msg_ut;

typedef struct mb_prop_msg_s
{
    u32_t size;
    u32_t reqres;
    u32_t tag_data[];
} __attribute__((packed)) mb_prop_msg_st;

mb_msg_ut mb_read(u8_t const ch);
void mb_write(mb_msg_ut const msg);
