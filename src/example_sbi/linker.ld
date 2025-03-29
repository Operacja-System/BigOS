/* Copyright (c) 2020 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */
OUTPUT_ARCH("riscv")

ENTRY(_enter)


PHDRS {
    text PT_LOAD;
    data PT_LOAD;
}

SECTIONS
{
    __stack_size = DEFINED(__stack_size) ? __stack_size : 32M;

    .init 0x80200000 : {
        *(.init.enter)
        *(SORT_NONE(.init))
    } :text

    .fini : {
        *(SORT_NONE(.fini))
    } :text

    .text : {
        *(.text.unlikely .text.unlikely.*)
        *(.text.startup .text.startup.*)
        *(.text .text.*)
        *(.gnu.linkonce.t.*)
    } :text

    .preinit_array : ALIGN(8) {
        __preinit_array_start = .;
        *(.preinit_array)
        __preinit_array_end = .;
    } :data

    .init_array : ALIGN(8) {
        __init_array_start = .;
        *(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*))
        *(.init_array .ctors)
        __init_array_end = .;
    } :data

    .fini_array : ALIGN(8) {
        __fini_array_start = .;
        *(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*))
        *(.fini_array .dtors)
        __fini_array_end = .;
    } :data

    .rodata : {
        *(.rdata)
        *(.rodata .rodata.*)
        *(.gnu.linkonce.r.*)
    } :data

    .data : {
        *(.data .data.*)
        *(.gnu.linkonce.d.*)
    } :data

    .sdata : {
        __global_pointer$ = . + 0x800;
        *(.srodata.cst16)
        *(.srodata.cst8)
        *(.srodata.cst4)
        *(.srodata.cst2)
        *(.srodata .srodata.*)
        *(.sdata .sdata.* .sdata2.*)
        *(.gnu.linkonce.s.*)
    } :data

    .bss (NOLOAD): {
         __bss_start = .;
        *(.sbss*)
        *(.gnu.linkonce.sb.*)
        *(.bss .bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        __bss_end = .;
    } :data

    .stack (NOLOAD) : ALIGN(16) {
        . += __stack_size;
        __stack_start = .;
    } :data

    /DISCARD/ : {
        *(.eh_frame .eh_frame.*)
    }
}
