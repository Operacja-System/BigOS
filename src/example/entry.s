.section .init

.option norvc

.type start, @function
.global start
start:
	.cfi_startproc

.option push
.option norelax
	la gp, global_pointer
.option pop

	/* Setup stack */
	la sp, stack_top

	/* Clear the BSS section */
	la t5, bss_start
	la t6, bss_end
bss_clear:
	sd zero, (t5)
	addi t5, t5, 8
	bltu t5, t6, bss_clear

	/* Jump to kernel! */
	jal ra, example_main

halt:
	wfi
	j halt

	.cfi_endproc

.end
