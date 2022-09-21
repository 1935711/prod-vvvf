#pragma once

#define FB_DB_TOGGLE 1

#include "rp.h"

#define FB_W 416U
#define FB_H 234U
#define FB_D 3U /* 3B = 24b = BGR. */

typedef struct fb_s
{
    u32_t width;
    u32_t height;
    u32_t vwidth;
    u32_t vheight;
    u32_t pitch; /* Number of bytes in each row of screen. */
    u32_t depth; /* Bits per pixel. */
    u32_t voffx;
    u32_t voffy;
    u8_t (*pointer)[FB_H * 2U][FB_W][FB_D];
    u32_t size;

    /* Extra user data (not in docs). */
    u32_t draw_off_y;
} __attribute__((packed)) fb_st;

typedef enum fb_buf_e
{
    FB_BUF_A, /* Active/selected buffer. */
    FB_BUF_0, /* Buffer 0 (y offset 0). */
    FB_BUF_1, /* Buffer 1 (y offset is FB height). */
} fb_buf_et;

extern fb_st __attribute__((aligned(16))) fb;

void fb_init(void);
void fb_buf_swap(void);
void fb_clr(void);

#define FB_DRAW_PIX(buf, x, y, r, g, b)                                        \
    do                                                                         \
    {                                                                          \
        volatile u8_t(*pix)[FB_D];                                             \
        if (buf == FB_BUF_A)                                                   \
        {                                                                      \
            pix = &(*fb.pointer)[y + fb.draw_off_y][x];                        \
        }                                                                      \
        else if (buf == FB_BUF_0)                                              \
        {                                                                      \
            pix = &(*fb.pointer)[y][x];                                        \
        }                                                                      \
        else if (buf == FB_BUF_1)                                              \
        {                                                                      \
            pix = &(*fb.pointer)[y + FB_H][x];                                 \
        }                                                                      \
        (*pix)[0U] = b;                                                        \
        (*pix)[1U] = g;                                                        \
        (*pix)[2U] = r;                                                        \
    } while (0U);
