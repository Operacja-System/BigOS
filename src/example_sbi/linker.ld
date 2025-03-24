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
        KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
        PROVIDE_HIDDEN (__init_array_end = .);
    }

    .fini_array : ALIGN(8) {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
        KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
        PROVIDE_HIDDEN (__fini_array_end = .);
    }

    .ctors : {
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
    }

    .dtors : {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        KEEP (*(.metal.dtors .metal.dtors.*))
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

    .tdata : {
        PROVIDE( __tls_base = . );
        *(.tdata .tdata.* .gnu.linkonce.td.*)
    }

    PROVIDE( __tdata_source = LOADADDR(.tdata) );
    PROVIDE( __tdata_size = SIZEOF(.tdata) );

    PROVIDE( metal_segment_data_source_start = LOADADDR(.data) );
    PROVIDE( metal_segment_data_target_start = ADDR(.data) );
    PROVIDE( metal_segment_data_target_end = ADDR(.tdata) + SIZEOF(.tdata) );

    .tbss : {
        *(.tbss .tbss.* .gnu.linkonce.tb.*)
        *(.tcommon .tcommon.*)
        PROVIDE( __tls_end = . );
    }
    PROVIDE( __tbss_size = SIZEOF(.tbss) );
    PROVIDE( __tls_size = __tls_end - __tls_base );

    .tbss_space : {
         . = . + __tbss_size;
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
