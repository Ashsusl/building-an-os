ENTRY(start)

SECTIONS
{
    . = 1M;

    .multiboot : ALIGN(4K)
    {
        KEEP(*(.multiboot_header))
    }

    .text : ALIGN(4K)
    {
        *(.text)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata)
    }

    .data : ALIGN(4K)
    {
        *(.data)
    }

    .bss : ALIGN(4K)
    {
        __bss_start = .;
        *(COMMON)
        *(.bss)
        --bss_end = .;
    }

    .idt BLOCK(0x1000) : ALIGN(0x1000)
    {
        IDT = .;
        . = . + 0x1000;
    }
}
