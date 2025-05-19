.section .init

	.option norvc

	.type start, @function
	.global start

	start:
		.cfi_startproc

		.option push
		.option norelax
		.option pop

		/* Setup stack */
		la sp, stack_end

		/* Jump to kbootstrap! */
		jal ra, kbootstrap

	halt:
		wfi
		j halt

	.cfi_endproc
	.end
