#include "uart0.h"
#include "gpio.h"
#include "rp.h"

void uart0_init(void)
{
#if UART0_TOGGLE
    volatile u32_t *const uart0_cr = RP_P_ADDR(RP_P_UART0_BASE, UART0_OFF_CR);
    volatile u32_t *const uart0_icr = RP_P_ADDR(RP_P_UART0_BASE, UART0_OFF_ICR);
    volatile u32_t *const uart0_ibrd =
        RP_P_ADDR(RP_P_UART0_BASE, UART0_OFF_IBRD);
    volatile u32_t *const uart0_fbrd =
        RP_P_ADDR(RP_P_UART0_BASE, UART0_OFF_FBRD);
    volatile u32_t *const uart0_lcrh =
        RP_P_ADDR(RP_P_UART0_BASE, UART0_OFF_LCRH);
    volatile u32_t *const uart0_imsc =
        RP_P_ADDR(RP_P_UART0_BASE, UART0_OFF_IMSC);

    /* Disable UART. */
    *uart0_cr = 0U;

    /* Disable FIFO to flush it. */
    *uart0_lcrh = 0U;

    /* Make pins 14 and 15 floating. */
    gpio_cfg_pull(1 << 14 | 1 << 15, GPIO_PULL_OFF);
    /* Select ALT0 for pins 14 (TXD0) and 15 (RXD0). */
    gpio_cfg_func(1 << 14 | 1 << 15, GPIO_FUNC_0);

    /* Create a 40 MHz UART0 clock. */
    *uart0_ibrd = 1U;
    *uart0_fbrd = 40U;

    /* Clear all supported interrupts. */
    *uart0_icr = 0x000003F2;

    /**
     * LSB>MSB
     *  1b =  0 = Output high on TXD (BRK)
     *  1b =  0 = Parity disable (PEN)
     *  1b =  0 = Odd parity (EPS)
     *  1b =  0 = Two stop bits not sent (STP2)
     *  1b =  1 = Enable FIFOs (FEN)
     *  2b = 01 = Word length is 7 bits (WLEN)
     *  1b =  0 = Stick parity is disabled (SPS, RO)
     * 23b =  0 = Reserved
     */
    *uart0_lcrh = 1 << 5 | 1 << 6;

    /**
     * LSB>MSB
     *  1b = 0 = Unsupported (RIMIM)
     *  1b = 1 = Mask CTSMIM
     *  1b = 0 = Unsupported (DCDMIM)
     *  1b = 0 = Unsupported (DSRMIM)
     *  1b = 1 = Mask RXIM
     *  1b = 1 = Mask TXIM
     *  1b = 1 = Mask RTIM
     *  1b = 1 = Mask FEIM
     *  1b = 1 = Mask PEIM
     *  1b = 1 = Mask BEIM
     *  1b = 1 = Mask OEIM
     * 20b = 0 = Reserved
     */
    /* Mask all interrupts. */
    *uart0_imsc = 0x07F2;

    /**
     * LSB>MSB
     *  1b = 1 = Enable (UART enable)
     *  1b = 0 = Unsupported (SIREN)
     *  1b = 0 = Unsupported (SIRLP)
     *  3b = 0 = Reserved
     *  1b = 0 = Disable (Loopback enable)
     *  1b = 1 = Enable (transmit enable)
     *  1b = 1 = Enable (receive enable)
     *  1b = 0 = Unsupported (DTR)
     *  1b = 0 = No (request to send)
     *  1b = 0 = Unsupported (OUT1)
     *  1b = 0 = Unsupported (OUT2)
     *  1b = 0 = Disable (RTS hardware flow control enable)
     *  1b = 0 = Disable (CTS hardware flow control enable)
     * 15b = 0 = Reserved
     */
    *uart0_cr = 1 << 0 | 1 << 8 | 1 << 9;
#endif
}

void uart0_putc(char const c)
{
#if UART0_TOGGLE
    volatile u32_t *const uart0_data =
        RP_P_ADDR(RP_P_UART0_BASE, UART0_OFF_DATA);
    volatile u32_t *const uart0_fr = RP_P_ADDR(RP_P_UART0_BASE, UART0_OFF_FR);

    /* Wait while transmit FIFO is full. */
    while (*uart0_fr & (1 << 5))
        ;

    /* Clear all errors and write the character. */
    *uart0_data = 0x00000000 | c;
#else
    (void)c;
#endif
}

void uart0_puts(volatile char const *const s)
{
#if UART0_TOGGLE
    u32_t idx = 0U;
    do
    {
        uart0_putc(s[idx]);
    } while (s[idx++] != '\0');
#else
    (void)s;
#endif
}

void uart0_getc(volatile char *const c)
{
#if UART0_TOGGLE
    volatile u32_t *const uart0_data =
        RP_P_ADDR(RP_P_UART0_BASE, UART0_OFF_DATA);
    volatile u32_t *const uart0_fr = RP_P_ADDR(RP_P_UART0_BASE, UART0_OFF_FR);

    /* Wait while receive FIFO is empty. */
    while (*uart0_fr & (1 << 4))
        ;

    /* Clear all errors and write the character. */
    *c = *uart0_data & 0x0000007F;
#else
    (void)c;
#endif
}

void uart0_gets(volatile char *const s, u32_t const len)
{
#if UART0_TOGGLE
    for (u32_t idx = 0; idx < len; ++idx)
    {
        uart0_getc(&s[idx]);
    }
#else
    (void)s;
    (void)len;
#endif
}
