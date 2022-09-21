#pragma once

#include "rp.h"

#define CM_PWD 0x5A

#define CM_OFF_PWM_CTL 0xA0
#define CM_OFF_PWM_DIV 0xA4

typedef union cm_ctl_u {
    struct
    {
        u8_t src : 4;
        u8_t enab : 1;
        u8_t kill : 1;
        u8_t reserved0 : 1;
        u8_t busy : 1;
        u8_t busyd : 1;
        u8_t mash : 2;
        u16_t reserved1 : 13;
        u8_t pwd : 8;
    } __attribute__((packed)) f;
    u32_t v;
} cm_ctl_ut;

typedef union cm_div_u {
    struct
    {
        u16_t divf : 12;
        u16_t divi : 12;
        u8_t pwd : 8;
    } __attribute__((packed)) f;
    u32_t v;
} cm_div_ut;
