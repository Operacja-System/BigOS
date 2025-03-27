/* Copyright (c) 2020 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */
OUTPUT_ARCH("riscv")

ENTRY(_enter)

SECTIONS
{
    __stack_size = DEFINED(__stack_size) ? __stack_size : 32M;
    PROVIDE(__stack_size = __stack_size);

    .init 0x80200000 : {
        KEEP (*(.init.enter))
        KEEP (*(SORT_NONE(.init)))
    }

    .fini : {
        KEEP (*(SORT_NONE(.fini)))
    }

    .preinit_array : ALIGN(8) {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    }

    .init_array : ALIGN(8) {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP (*(.init_array .ctors))
        PROVIDE_HIDDEN (__init_array_end = .);
    }

    .fini_array : ALIGN(8) {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
        KEEP (*(.fini_array .dtors))
        PROVIDE_HIDDEN (__fini_array_end = .);
    }

    .rodata : {
        *(.rdata)
        *(.rodata .rodata.*)
        *(.gnu.linkonce.r.*)
        . = ALIGN(8);
        *(.srodata.cst16)
        *(.srodata.cst8)
        *(.srodata.cst4)
        *(.srodata.cst2)
        *(.srodata .srodata.*)
    }

    .text : {
        *(.text.unlikely .text.unlikely.*)
        *(.text.startup .text.startup.*)
        *(.text .text.*)
        *(.gnu.linkonce.t.*)
    }

    .data : ALIGN(8) {
        *(.data .data.*)
        *(.gnu.linkonce.d.*)
        . = ALIGN(8);
        PROVIDE( __global_pointer$ = . + 0x800 );
        *(.sdata .sdata.* .sdata2.*)
        *(.gnu.linkonce.s.*)
    }

    .bss (NOLOAD): {
        PROVIDE( __bss_start = . );
        *(.sbss*)
        *(.gnu.linkonce.sb.*)
        *(.bss .bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        PROVIDE( __bss_end = . );
    }

    .stack (NOLOAD) : ALIGN(16) {
        . += __stack_size;
        PROVIDE( _sp = . );
    }

    /DISCARD/ : {
        *(.eh_frame .eh_frame.*)
    }
}
