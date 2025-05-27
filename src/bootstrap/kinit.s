
.section .bss
	.space 0x10000
stack_end:

.section .init

	.option norvc

	.type _start, @function
	.global _start

	_start:
		.cfi_startproc

		/* Setup stack */
		la sp, stack_end

		/* Jump to kbootstrap! */
		jal ra, kbootstrap

	halt:
		wfi
		j halt

	.cfi_endproc
	.end
