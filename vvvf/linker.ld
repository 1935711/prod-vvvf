SECTIONS
{
    /**
     * Jumped to from armstub.S
     * https://github.com/raspberrypi/tools/blob/master/armstubs/armstub.S#L35
     */
    .text 0x8000 :
    {
        KEEP(*(.text.boot))
        *(.text .text.* .gnu.linkonce.t*)
    }

    . = ALIGN(16);
    .rodata :
    {
        *(.rodata .rodata.* .gnu.linkonce.r*)
    }

    . = ALIGN(16);
    .data :
    {
        *(.data .data.* .gnu.linkonce.d*)
    }

    . = ALIGN(16);
    __bss_start = .;
    .bss (NOLOAD) :
    {
        *(.bss .bss.*)
    }
    . = ALIGN(16);
    __bss_end = .;
    __bss_size = __bss_end - __bss_start;

    /DISCARD/ : {
        *(.comment)
        *(.gnu*)
        *(.note*)
        *(.eh_frame*)
    }
}
