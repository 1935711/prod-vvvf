#include "fb.h"
#include "rp.h"
#include "text.h"

static void irq_start(char const *const txt)
{
    text_draw(0, FB_H - 15, 3U, txt, UINT8_MAX, UINT8_MAX, UINT8_MAX, 1U);
}

void irq_reset(void)
{
    irq_start("IRQ - RST");
    panic();
}

void irq_insundef(void)
{
    irq_start("IRQ - OPU");
    panic();
}

void irq_swi(void)
{
    irq_start("IRQ - SWI");
    panic();
}

void irq_prefetchabort(void)
{
    irq_start("IRQ - PAB");
    panic();
}

void irq_dataabort(void)
{
    irq_start("IRQ - DAB");
    panic();
}
