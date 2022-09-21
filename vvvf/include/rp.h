#pragma once

typedef unsigned long long u64_t;
typedef signed long long i64_t;
typedef unsigned int u32_t;
typedef signed int i32_t;
typedef unsigned short u16_t;
typedef signed short i16_t;
typedef unsigned char u8_t;
typedef signed char i8_t;
_Static_assert(sizeof(u64_t) == 8U, "u64 is not 8 bytes wide.");
_Static_assert(sizeof(i64_t) == 8U, "i64 is not 8 bytes wide.");
_Static_assert(sizeof(u32_t) == 4U, "u32 is not 4 bytes wide.");
_Static_assert(sizeof(i32_t) == 4U, "i32 is not 4 bytes wide.");
_Static_assert(sizeof(u16_t) == 2U, "u16 is not 2 bytes wide.");
_Static_assert(sizeof(i16_t) == 2U, "i32 is not 2 bytes wide.");
_Static_assert(sizeof(u8_t) == 1U, "u8 is not 1 byte wide.");
_Static_assert(sizeof(i8_t) == 1U, "i8 is not 1 byte wide.");

#define UINT8_MAX (255U)
#define UINT16_MAX (65535U)
#define UINT32_MAX (4294967295U)
#define UINT64_MAX (18446744073709551615U)

#define INT8_MAX (127)
#define INT16_MAX (32767)
#define INT32_MAX (2147483647)
#define INT64_MAX (9223372036854775807)

/* The state word must be initialized to non-zero. */
typedef struct xorsh32_s
{
    u32_t a;
} xorsh32_st;

#define RP_VC_L2 1U

/* These are the physical addresses of the ARM. */
#if RP_VC_L2 == 1U
#define RP_VC_ADDR_ZERO 0x40000000
#else
#define RP_VC_ADDR_ZERO 0xC0000000
#endif

/* Start of peripheral memory in ARM space (physical address space). */
#define RP_P_PHY 0x20000000
/* Peripheral bus address range start address. */
#define RP_P_BUS_ARM 0x7E000000
#define RP_P_BUS_DMA 0xC0000000

#define RP_P_BUS_ADDR(phy, bus) ((phy & 0x00FFFFFF) | bus)

/* Create an ARM physical address for accessing a register of a peripheral. */
#define RP_P_ADDR(base, off) ((volatile u32_t *)(RP_P_PHY + base + off))

#define RP_P_MB_BASE 0x00B880
#define RP_P_UART0_BASE 0x201000
#define RP_P_GPIO_BASE 0x200000
#define RP_P_PCM_BASE 0x203000
#define RP_P_PWM_BASE 0x20C000
#define RP_P_CM_BASE 0x101000
#define RP_P_DMA_BASE(ch) (ch == 15 ? 0xE05000 : 0x007000 + (0x100 * ch))

u32_t xorshift32(xorsh32_st *const state);
void __attribute__((noreturn)) panic(void);
void delay(u32_t const cycles);
