ENTRY(start);


SECTIONS {
	. = 0x80000000;
	PROVIDE(ram_start = .);
	PROVIDE(kernel_start = .);
	.text : ALIGN(2M) {
		*(.init);
		*(.text);
	}
	. = ALIGN(4K);
	PROVIDE(global_ptr = .);
	.rodata : {
		*(.rodata);
	}
	.data : ALIGN(4K) {
		*(.data);
	}
	.bss : ALIGN(4K) {
		PROVIDE(bss_start = .);
		*(.bss);
		*(COMMON)
		PROVIDE(bss_end = .);
	}
	. = ALIGN(2M);
	PROVIDE(kernel_end = .);
	PROVIDE(stack_start = .);
	. += 2M;
	PROVIDE(stack_end = .);
}
