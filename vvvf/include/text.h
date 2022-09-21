#pragma once

#include "rp.h"

void text_draw(u16_t const x, u16_t const y, u8_t const scale,
               volatile char const *const text, u8_t const r, u8_t const g,
               u8_t const b, u8_t const clear);
void text_hex(u32_t const n, u8_t const w, volatile char *const s);
void text_dec(u32_t const n, u8_t const w, volatile char *const s);
