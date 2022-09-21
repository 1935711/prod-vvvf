#include "pbar.h"
#include "fb.h"
#include "text.h"
#include "uart0.h"

#define PBAR_H 50U
#define PBAR_W (FB_W - 50U)
#define PBAR_Y 30U
#define PBAR_X ((FB_W - PBAR_W) / 2U)
#define PBAR_MAX 100U

void pbar(u8_t const progress, char const id)
{
    u16_t const pwm_len_on = progress;
    u16_t const pwm_len_off = (100U - progress);

    u8_t on_off = 0U;

    char loading_text[] = "LOADING... 0 000";
    loading_text[11U] = id;

    text_dec(progress, 3U, &loading_text[13U]);
    text_draw(PBAR_X, PBAR_Y - 20U, 2U, loading_text, UINT8_MAX, UINT8_MAX,
              UINT8_MAX, 1U);

    for (u16_t pbar_h = 0U; pbar_h < PBAR_H; ++pbar_h)
    {
        for (u16_t pbar_w = 0U; pbar_w < PBAR_W; ++pbar_w)
        {
            FB_DRAW_PIX(FB_BUF_A, PBAR_X + pbar_w, PBAR_Y + pbar_h, 0U, 0U, 0U);
        }
    }

    for (u16_t pbar_w = 0U; pbar_w < PBAR_W;)
    {
        u16_t const pwm_len_on_off = (on_off == 1U ? pwm_len_on : pwm_len_off);
        u16_t const x = PBAR_X + pbar_w;
        u8_t const color = UINT8_MAX;
        u16_t pwm_len = 0U;
        u16_t const pwm_w =
            (((pwm_len_on_off + x) - PBAR_X <= PBAR_W) ? pwm_len_on_off
                                                       : PBAR_W - x + PBAR_X);
        for (; pwm_len < pwm_w; ++pwm_len)
        {
            u16_t const y = on_off % 2U == 0U ? PBAR_Y + PBAR_H - 1U : PBAR_Y;
            u8_t const r = pwm_len + 64U, g = color - 20U, b = color;
            FB_DRAW_PIX(FB_BUF_A, x + pwm_len, y, r, g, b);
            FB_DRAW_PIX(FB_BUF_A, x + pwm_len, y - 1U, r, g, b);
        }
        if (on_off == 1U && !(pwm_len_on == PBAR_MAX || pwm_len_on == 0U))
        {
            for (u16_t pwm_h = 0U; pwm_h < PBAR_H; ++pwm_h)
            {
                u8_t const r = pwm_len + 64U, g = color - 20U, b = color;
                u16_t const y = PBAR_Y + pwm_h;
                FB_DRAW_PIX(FB_BUF_A, x, y, r, g, b);
                FB_DRAW_PIX(FB_BUF_A, x - 1U, y, r, g, b);
                if (pwm_w == pwm_len_on_off)
                {
                    FB_DRAW_PIX(FB_BUF_A, x + pwm_len, y, r, g, b);
                    FB_DRAW_PIX(FB_BUF_A, x + pwm_len + 1U, y, r, g, b);
                }
            }
        }
        pbar_w += pwm_w;
        on_off += 1U;
        on_off %= 2U;
    }

    char const *info_text;
    if (progress < 20U)
    {
        info_text = "VVVF BY 1935711";
    }
    else if (progress < 28U)
    {
        info_text = "HELLO XENIUM!               \r\n"
                    "IM A NEWB SO THIS IS THE    \r\n"
                    "FIRST IRL PARTY FOR ME          ";
    }
    else if (progress < 40U)
    {
        info_text = "PRE-CALC WILL LAST FOR      \r\n"
                    "30 SECONDS SO ILL CHAT WITH \r\n"
                    "YA NOW                          ";
    }
    else if (progress < 50U)
    {
        info_text = "THIS INTRO IS RUNNING ON A  \r\n"
                    "RASPBERRY PI 1B AT 900 MHZ  \r\n"
                    "BAREMETAL + C + ASM             ";
    }
    else if (progress < 68U)
    {
        info_text = "THE GPU IS COMPLICATED     \r\n"
                    "SO NO ACCELERATION         \r\n"
                    "FOR NOW...                     ";
    }
    else if (progress < 78U)
    {
        info_text = "VVVF = VARIABLE VOLTAGE    \r\n"
                    "VARIABLE FREQUENCY DRIVE   \r\n"
                    "                           \r\n"
                    "SOME TRAINS USE IT HENCE   \r\n"
                    "THE GEAR-CHANGE SOUND          ";
    }
    else
    {
        info_text = "GOT A BUNCH OF COOL EFFECTS\r\n"
                    "THOUGH SO HOPE YOU ENJOY!!!\r\n"
                    "FIRST UP A VVVF SOUND!     \r\n"
                    "                           \r\n"
                    "                               ";
    }
    u8_t const color = progress * 2U + 55U;
    text_draw(30U, FB_H - 100U, 3U, info_text, color, color, color, 1U);
}

void pbar_delta(u8_t const progress, char const id)
{
    static u8_t progress_state = 0U;
    if (progress_state + progress <= PBAR_MAX)
    {
        progress_state += progress;
        pbar(progress_state, id);
    }
    else
    {
        pbar(PBAR_MAX, id);
    }
}
