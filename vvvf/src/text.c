#include "fb.h"
#include "rp.h"
#include "uart0.h"

#define FONT_W 3U
#define FONT_H 5U

/* 3x5 matrix display font. */
static u16_t const font_alpha[] = {
    0x5BEF, /* A */
    0x7AEF, /* B */
    0x724F, /* C */
    0x3B6B, /* D */
    0x72CF, /* E */
    0x12CF, /* F */
    0x7B4F, /* G */
    0x5BED, /* H */
    0x7497, /* I */
    0x7B26, /* J */
    0x5AED, /* K */
    0x7249, /* L */
    0x5B7D, /* M */
    0x5B6F, /* N */
    0x7B6F, /* O */
    0x13EF, /* P */
    0x4F6F, /* Q */
    0x5AEF, /* R */
    0x79CF, /* S */
    0x2497, /* T */
    0x7B6D, /* U */
    0x176D, /* V */
    0x5F6D, /* W */
    0x5AAD, /* X */
    0x24AD, /* Y */
    0x72A7, /* Z */
};
static u16_t const font_numeric[] = {
    0x7B6F, /* 0 */
    0x4926, /* 1 */
    0x73E7, /* 2 */
    0x79A7, /* 3 */
    0x49ED, /* 4 */
    0x79CF, /* 5 */
    0x7BCF, /* 6 */
    0x4927, /* 7 */
    0x7BEF, /* 8 */
    0x79EF, /* 9 */
};

void text_draw(u16_t const x, u16_t const y, u8_t const scale,
               volatile char const *const text, u8_t const r, u8_t const g,
               u8_t const b, u8_t const clear)
{
    if (!fb.pointer)
    {
        uart0_puts(text);
        uart0_puts("\r\n");
        return;
    }

    volatile char const *c_ptr = text;
    char ch;
    u32_t text_idx = 0U;
    u32_t offset_x = 0U;
    u32_t offset_y = 0U;
    while ((ch = c_ptr[text_idx]) != '\0')
    {
        u32_t font_glyph;
        if (ch >= '0' && ch <= '9')
        {
            ch -= '0';
            font_glyph = font_numeric[(u8_t)ch];
        }
        else if (ch >= 'A' && ch <= 'Z')
        {
            ch -= 'A';
            font_glyph = font_alpha[(u8_t)ch];
        }
        else if (ch == ' ')
        {
            font_glyph = 0x0000;
        }
        else if (ch == '\r')
        {
            offset_x = 0U;
            text_idx += 1U;
            continue;
        }
        else if (ch == '\n')
        {
            offset_y += (FONT_H * (scale + 1U));
            text_idx += 1U;
            continue;
        }
        else if (ch == '-')
        {
            font_glyph = 0x01C0;
        }
        else if (ch == '+')
        {
            font_glyph = 0x05D0;
        }
        else if (ch == ':')
        {
            font_glyph = 0x0410;
        }
        else if (ch == '=')
        {
            font_glyph = 0x0E38;
        }
        else if (ch == '.')
        {
            font_glyph = 0x2000;
        }
        else if (ch == '!')
        {
            font_glyph = 0x2092;
        }
        else
        {
            font_glyph = 0x7B6F;
        }

        for (u8_t i = 0U; i < 15U; ++i)
        {
            u8_t const seg = font_glyph & 0x00000001;
            u8_t const draw = seg ? 255U : 0U;
            font_glyph >>= 1;

            for (u8_t scale_y = 0U; scale_y < scale; ++scale_y)
            {
                for (u8_t scale_x = 0U; scale_x < scale; ++scale_x)
                {
                    u16_t const x_common = x + offset_x + scale_x;
                    u16_t const y_common = y + offset_y + scale_y;
                    if (draw)
                    {
                        FB_DRAW_PIX(FB_BUF_A, x_common + ((i % 3U) * scale),
                                    y_common + ((i / 3U) * scale), r, g, b);
                    }
                    else if (clear)
                    {
                        FB_DRAW_PIX(FB_BUF_A, x_common + ((i % 3U) * scale),
                                    y_common + ((i / 3U) * scale), 0U, 0U, 0U);
                    }
                }
            }
        }

        offset_x += (FONT_W * scale) + 4U;
        text_idx += 1U;
    }
}

void text_hex(u32_t const n, u8_t const w, volatile char *const s)
{
    u32_t n_tmp = n;
    for (u8_t i = 0U; i < w * 2U; ++i)
    {
        u8_t const nibble = n_tmp & 0x0000000F;
        s[(w * 2U) - 1U - i] =
            nibble > 0x09 ? (nibble - 0x0A) + 65U : nibble + 48U;
        n_tmp >>= 4;
    }
}

void text_dec(u32_t const n, u8_t const w, volatile char *const s)
{
    u32_t n_cpy = n;
    u8_t i = 0U;
    while (n_cpy > 0U && i < w)
    {
        u8_t const digit = n_cpy % 10U;
        n_cpy /= 10U;
        s[w - i - 1U] = '0' + digit;
        ++i;
    }
}
