ENTRY(_enter);

. = 0x80000000;

SECTIONS {
	/* Include entry point at start of binary */
	.text : ALIGN(4K) {
		*(.init.enter);
		*(.init);
		*(.text);
	}
	.bss : ALIGN(4K) {
		PROVIDE(__bss_start = .);
		*(.bss);
		*(COMMON);
		PROVIDE(__global_pointer$ = .);
		PROVIDE(__bss_end = .);
	}
	.stack : ALIGN(8) {
		. += 4096;
		PROVIDE(stack_top = .);
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
	.rodata : ALIGN(4K) {
		*(.rodata);
	}
	.data : ALIGN(4K) {
		*(.data);
	}
}
