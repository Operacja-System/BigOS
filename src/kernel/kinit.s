.section .init

	.option norvc

	.type start, @function
	.global start

	start:
		.cfi_startproc

		.option push
		.option norelax
		la gp, global_ptr
		.option pop

		/* Setup stack */
		la sp, stack_end

		/* Clear the BSS section */
		la t5, bss_start
		la t6, bss_end
		bss_clear:
			sd zero, (t5)
			addi t5, t5, 8
			bltu t5, t6, bss_clear

		/* Jump to kinit! */
		/* kinit args: 
			a0 - kernel_address
			a1 - kernel_size
			a2 - kernel_stack_address
			a3 - kernel_stack_size
			a4 - ram_start
		*/
		la a0, kernel_start
		la t0, kernel_end
		sub a1, t0, a0
		la a2, stack_start
		la t0, stack_end
		sub a3, t0, a2
		la a4, ram_start
		jal ra, kinit

	halt:
		wfi
		j halt

	.cfi_endproc
	.end
