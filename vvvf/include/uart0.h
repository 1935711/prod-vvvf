#pragma once
/* UART0 = PL011 UART */

#include "rp.h"

#define UART0_TOGGLE 1U

#define UART0_OFF_DATA 0x00 /* Data register. */
#define UART0_OFF_RSRECR 0x04
#define UART0_OFF_FR 0x18   /* Flag register. */
#define UART0_OFF_ILPR 0x20 /* Not used. */
#define UART0_OFF_IBRD 0x24 /* Integer baud rate divisor. */
#define UART0_OFF_FBRD 0x28 /* Fractional baud rate divisor. */
#define UART0_OFF_LCRH 0x2C /* Line control register. */
#define UART0_OFF_CR 0x30   /* Control register. */
#define UART0_OFF_IFLS 0x34 /* Interrupt FIFO level select register. */
#define UART0_OFF_IMSC 0x38 /* Interrupt mask set clear register. */
#define UART0_OFF_RIS 0x3C  /* Raw interrupt status register. */
#define UART0_OFF_MIS 0x40  /* Masked interrupt status register. */
#define UART0_OFF_ICR 0x44  /* Interrupt clear register. */

#define UART0_OFF_DMACR 0x48 /* DMA control register (disabled). */
#define UART0_OFF_ITCR 0x80  /* Test control register. */
#define UART0_OFF_ITIP 0x84  /* Integration test input register. */
#define UART0_OFF_ITOP 0x88  /* Integration test output register. */
#define UART0_OFF_TDR 0x8C   /* Test data register. */

void uart0_init();

void uart0_putc(char const c);
void uart0_puts(volatile char const *const s);

void uart0_getc(volatile char *const c);
void uart0_gets(volatile char *const s, u32_t const len);
