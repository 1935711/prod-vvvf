#pragma once
#include "rp.h"

/* Function select. */
#define GPIO_OFF_FSEL0 0x00
#define GPIO_OFF_FSEL1 0x04
#define GPIO_OFF_FSEL2 0x08
#define GPIO_OFF_FSEL3 0x0C
#define GPIO_OFF_FSEL4 0x10
#define GPIO_OFF_FSEL5 0x14

/* Pin output set. */
#define GPIO_OFF_SET0 0x1C
#define GPIO_OFF_SET1 0x20

/* Pin output clear. */
#define GPIO_OFF_CLR0 0x28
#define GPIO_OFF_CLR1 0x2C

/* Pin level. */
#define GPIO_OFF_LEV0 0x34
#define GPIO_OFF_LEV1 0x38

/* Pin event detect status. */
#define GPIO_OFF_EDS0 0x40
#define GPIO_OFF_EDS1 0x44

/* Pin rising edge detect enable. */
#define GPIO_OFF_REN0 0x4C
#define GPIO_OFF_REN1 0x50

/* Pin falling edge detect enable. */
#define GPIO_OFF_FEN0 0x58
#define GPIO_OFF_FEN1 0x5C

/* Pin high detect enable. */
#define GPIO_OFF_HEN0 0x64
#define GPIO_OFF_HEN1 0x68

/* Pin low detect enable. */
#define GPIO_OFF_LEN0 0x70
#define GPIO_OFF_LEN1 0x74

/* Pin async. Rising edge detect. */
#define GPIO_OFF_AREN0 0x7C
#define GPIO_OFF_AREN1 0x80

/* Pin async. Falling edge detect. */
#define GPIO_OFF_AFEN0 0x88
#define GPIO_OFF_AFEN1 0x8C

/* Pin pull-up/down enable. */
#define GPIO_OFF_PUD 0x94

/* Pin pull-up/down enable clock. */
#define GPIO_OFF_PUDCLK0 0x98
#define GPIO_OFF_PUDCLK1 0x9C

/* Test. */
#define GPIO_OFF_TEST 0xB0 /* Unlike the other 32b regs, this is 4b. */

#define GPIO_PIN_LED_ACT 16U
#define GPIO_PIN_PWM0_JACK 40U
#define GPIO_PIN_PWM1_JACK 45U

typedef enum gpio_func_e
{
    GPIO_FUNC_IN = 0x00,
    GPIO_FUNC_OUT = 0x01,
    GPIO_FUNC_0 = 0x04,
    GPIO_FUNC_1 = 0x05,
    GPIO_FUNC_2 = 0x06,
    GPIO_FUNC_3 = 0x07,
    GPIO_FUNC_4 = 0x03,
    GPIO_FUNC_5 = 0x02,
} gpio_func_et;

typedef enum gpio_pull_e
{
    GPIO_PULL_OFF = 0x00,
    GPIO_PULL_DOWN = 0x01,
    GPIO_PULL_UP = 0x02,
    GPIO_PULL_RESERVED = 0x03,
} gpio_pull_et;

typedef enum gpio_drv_e
{
    GPIO_DRV_HIGH,
    GPIO_DRV_LOW,
} gpio_drv_et;

void gpio_cfg_func(u64_t const pin_mask, gpio_func_et const func);
void gpio_cfg_pull(u64_t const pin_mask, gpio_pull_et const pull);
void gpio_cfg_drv(u8_t const pin_idx, gpio_drv_et const drv);
