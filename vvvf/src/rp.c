#include "rp.h"
#include "gpio.h"

u32_t xorshift32(xorsh32_st *const state)
{
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs". */
    u32_t x = state->a;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return state->a = x;
}

void __attribute__((noreturn)) panic(void)
{
    for (;;)
    {
        gpio_cfg_drv(GPIO_PIN_LED_ACT, GPIO_DRV_HIGH);
        delay(0x200000);
        gpio_cfg_drv(GPIO_PIN_LED_ACT, GPIO_DRV_LOW);
        delay(0x200000);
    }
}

void delay(u32_t const cycles)
{
    for (u32_t i = 0; i < (cycles / 2U); ++i)
    {
        asm volatile("nop");
    }
}
