ENTRY(start)

SECTIONS
{
	/* GDT fixée à 0x800 (en dessous du premier MB, hors BIOS) */
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
}