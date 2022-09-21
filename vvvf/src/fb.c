#include "fb.h"
#include "mb.h"
#include "text.h"

fb_st __attribute__((aligned(16))) fb = {
    .width = FB_W,
    .height = FB_H,
    .vwidth = FB_W,
    .vheight = FB_H * 2U,
    .pitch = 0, /* Initialized by GPU. */
    .depth = FB_D * 8U,
    .voffx = 0,
    .voffy = 0,
    .pointer = 0, /* Initialized by GPU. */
    .size = 0,    /* Initialized by GPU. */

    .draw_off_y = 0U,
};

void fb_init(void)
{
    mb_msg_ut fb_init = {
        .f =
            {
                .ch = MB_CH_FRAMEBUF,
                .data = ((u32_t)&fb + RP_VC_ADDR_ZERO) >>
                        4U, /* Data is the upper 28 bits so shift down by 1
                               nibble. */
            },
    };
    mb_write(fb_init);

    fb_init = mb_read(MB_CH_FRAMEBUF);
    if (fb_init.f.data != (u32_t)0)
    {
        panic();
    }

    fb.pointer -= RP_VC_ADDR_ZERO;
}

void fb_buf_swap(void)
{
#if FB_DB_TOGGLE == 1
    if (fb.voffy == 0U)
    {
        fb.draw_off_y = 0U;
        fb.voffy = FB_H;
    }
    else
    {
        fb.draw_off_y = FB_H;
        fb.voffy = 0U;
    }

    static u32_t __attribute__((aligned(16))) prop_buf[16U] = {0U};
    static mb_prop_msg_st *const prop_msg = (mb_prop_msg_st *)prop_buf;
    prop_msg->size = 16U;
    prop_msg->reqres = 0U;
    prop_msg->tag_data[0U] = 0x00048009; /* Set virtual screen offset. */
    prop_msg->tag_data[1U] = 8U;         /* Size of value buffer. */
    prop_msg->tag_data[2U] = 0U;         /* Request/response code. */
    prop_msg->tag_data[3U] = 0U;         /* Virtual screen offset X. */
    prop_msg->tag_data[4U] = fb.voffy;   /* Virtual screen offset Y. */
    prop_msg->tag_data[5U] = 0x00000000; /* End tag. */

    mb_msg_ut mb_msg = {
        .f =
            {
                .ch = MB_CH_PROPOUT,
                .data = ((u32_t)&prop_buf + RP_VC_ADDR_ZERO) >>
                        4U, /* Data is the upper 28 bits so shift down by 1
                               nibble. */
            },
    };
    mb_write(mb_msg);
    mb_msg = mb_read(MB_CH_PROPOUT);
    if (mb_msg.f.data != 0U && prop_msg->reqres == 0x80000000)
    {
        panic();
    }
#endif
}

void fb_clr(void)
{
    for (u16_t y = 0U; y < FB_H; ++y)
    {
        for (u16_t x = 0U; x < FB_W; ++x)
        {
            FB_DRAW_PIX(FB_BUF_0, x, y, 0U, 0U, 0U);
            FB_DRAW_PIX(FB_BUF_1, x, y, 0U, 0U, 0U);
        }
    }
}
