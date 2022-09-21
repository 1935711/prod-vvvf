#include "mb.h"
#include "rp.h"
#include "text.h"
#include "uart0.h"

mb_msg_ut mb_read(u8_t const ch)
{
    volatile u32_t *const status = RP_P_ADDR(RP_P_MB_BASE, MB_OFF_STATUS);
    volatile u32_t *const msg_read = RP_P_ADDR(RP_P_MB_BASE, MB_OFF_READ);

    mb_msg_ut msg_cpy;
    mb_status_ut status_cpy;
    do
    {
        do
        {
            status_cpy.v = *status;
        } while (status_cpy.f.empty);
        msg_cpy.v = *msg_read;
    } while (msg_cpy.f.ch != ch);

    return msg_cpy;
}

void mb_write(mb_msg_ut const msg)
{
    volatile u32_t *const status = RP_P_ADDR(RP_P_MB_BASE, MB_OFF_STATUS);
    volatile u32_t *const msg_write = RP_P_ADDR(RP_P_MB_BASE, MB_OFF_WRITE);

    mb_status_ut status_cpy;
    do
    {
        status_cpy.v = *status;
    } while (status_cpy.f.full);

    *msg_write = msg.v;
}
