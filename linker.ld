ENTRY(start)

SECTIONS
{
	/* GDT fixed at 0x800 (below first MB, outside BIOS) */
	.gdt 0x00000800 :
    {
        *(.gdt)
    }

    . = 1M;

    .text :
    {
        *(.multiboot)
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
		*(COMMON)
    }

	/* Tag where the compiled kernel ends in the RAM. */
    kernel_end = .;
}