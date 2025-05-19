
SECTIONS {
	. = 0x80000000;

	.boot.text : ALIGN(2M) {
		*(.boot.text);
		*(.init);
	}
	.boot.data : ALIGN(2M) {
		*(.boot.data);
	}
	PROVIDE(stack_start = .);
	. += 2M;
	PROVIDE(stack_end = .);

	. = 0x800000000000;
	.text : ALIGN(2M) {
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
}
