#include "gpio.h"

void gpio_cfg_func(u64_t const pin_mask, gpio_func_et const func)
{
    volatile u32_t *const gpio_fsel0 =
        RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_FSEL0);
    volatile u32_t *const gpio_fsel1 =
        RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_FSEL1);
    volatile u32_t *const gpio_fsel2 =
        RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_FSEL2);
    volatile u32_t *const gpio_fsel3 =
        RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_FSEL3);
    volatile u32_t *const gpio_fsel4 =
        RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_FSEL4);
    volatile u32_t *const gpio_fsel5 =
        RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_FSEL5);

    for (u8_t pin_idx = 0U; pin_idx < 53U; ++pin_idx)
    {
        /* Only set the function for pins present in the mask. */
        if (!(pin_mask & (1U << pin_idx)))
        {
            continue;
        }

        u32_t const fsel_mask_set =
            (((func >> 0U & 0x01) << (pin_idx * 2U)) + 0U) |
            (((func >> 1U & 0x01) << (pin_idx * 2U)) + 1U) |
            (((func >> 2U & 0x01) << (pin_idx * 2U)) + 2U);
        u32_t const fsel_mask_clr = ((1U << (pin_idx * 2U)) + 0U) |
                                    ((1U << (pin_idx * 2U)) + 1U) |
                                    ((1U << (pin_idx * 2U)) + 2U);

        volatile u32_t *gpio_fsel;
        if (pin_idx < 10)
        {
            gpio_fsel = gpio_fsel0;
        }
        else if (pin_idx < 20U)
        {
            gpio_fsel = gpio_fsel1;
        }
        else if (pin_idx < 30U)
        {
            gpio_fsel = gpio_fsel2;
        }
        else if (pin_idx < 40U)
        {
            gpio_fsel = gpio_fsel3;
        }
        else if (pin_idx < 50U)
        {
            gpio_fsel = gpio_fsel4;
        }
        else if (pin_idx < 60U)
        {
            gpio_fsel = gpio_fsel5;
        }
        else
        {
            panic();
        }

        /* First clear the function. */
        *gpio_fsel &= ~fsel_mask_clr;
        /* Second set the new function. */
        *gpio_fsel |= fsel_mask_set;
    }
}

void gpio_cfg_pull(u64_t const pin_mask, gpio_pull_et const pull)
{
    volatile u32_t *const gpio_pud = RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_PUD);
    volatile u32_t *const gpio_pudclk0 =
        RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_PUDCLK0);
    volatile u32_t *const gpio_pudclk1 =
        RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_PUDCLK1);

    *gpio_pud = pull;
    delay(150U);
    *gpio_pudclk0 |= 0x00000000FFFFFFFF & pin_mask;
    *gpio_pudclk1 |= 0x00000000FFFFFFFF & (pin_mask >> 32U);
    delay(150U);
    *gpio_pud = GPIO_PULL_OFF;
    *gpio_pudclk0 = 0U;
    *gpio_pudclk1 = 0U;
}

void gpio_cfg_drv(u8_t const pin_idx, gpio_drv_et const drv)
{
    volatile u32_t *const gpio_set0 = RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_SET0);
    volatile u32_t *const gpio_set1 = RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_SET1);
    volatile u32_t *const gpio_clr0 = RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_CLR0);
    volatile u32_t *const gpio_clr1 = RP_P_ADDR(RP_P_GPIO_BASE, GPIO_OFF_CLR1);

    volatile u32_t *gpio_set;
    volatile u32_t *gpio_clr;
    if (pin_idx > 31U)
    {
        gpio_set = gpio_set0;
        gpio_clr = gpio_clr0;
    }
    else
    {
        gpio_set = gpio_set1;
        gpio_clr = gpio_clr1;
    }

    if (drv == GPIO_DRV_HIGH)
    {
        *gpio_set |= 1U << (pin_idx % 31U);
    }
    else if (drv == GPIO_DRV_LOW)
    {
        *gpio_clr |= 1U << (pin_idx % 31U);
    }
}
