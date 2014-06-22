/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 * Indicate the architecture and format
 */
OUTPUT_ARCH(arm)
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")

/**
 * Indicate to the linker the entry point.
 */
ENTRY(Reset_Handler)

/**
 *   Linker script for Texas Instruments CC2538SF53 (32K RAM, 512K Flash)
 *   RAM is 16K RETENTION and 16K NON-RETENTION
 *   NON-RETENTION RAM starts at 0x20000000 with length 16K
 *   RETENTION RAM starts at 0x20004000 with length 16K
 *   Currently we are only using 16K with RETENTION
 */
MEMORY
{
    FLASH (RX) : ORIGIN = 0x00200000, LENGTH = 512K
    FLASH_CCA (RX) : ORIGIN = 0x0027FFD4, LENGTH = 12
    SRAM (RWX) : ORIGIN = 0x20004000, LENGTH = 16K
}

SECTIONS
{
    .text :
    {
        _text = .;
        . = ALIGN(4);
        KEEP(*(.vectors))
        
        . = ALIGN(4);
        *(.text*)
        *(.rodata*)
        
        . = ALIGN(4);
        KEEP(*(.init))

        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        . = ALIGN(4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;

        . = ALIGN(4);
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))
        
        _etext = .;
    } > FLASH= 0

    .data :
    {
        . = ALIGN(4);
        _data = .;
        *(vtable)
        *(.data*)
        _edata = .;
    } > SRAM AT > FLASH

    .ARM.exidx :
    {
        . = ALIGN(4);
        *(.ARM.exidx*)
    } > FLASH

    .bss :
    {
        . = ALIGN(4);
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM

    .flashcca :
    {
        . = ALIGN(4);
        KEEP(*(.flashcca))
    } > FLASH_CCA
    
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
}
