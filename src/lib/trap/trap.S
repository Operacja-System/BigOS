.text

.global trap_entry
.global trap_restore_with_cleanup
.global trap_utils_jump_with_stack

.equ SSTATUS_SIE, 0x00000002 // Supervisor Interrupt Enable
.equ SSTATUS_SPP, 0x00000100 // Supervisor Previous Privilege (1 = S-mode, 0 = U-mode)
.equ FRAME_SIZE, 0x110

// sscratch should have kernel stack pointer IIF in user-mode
// when coming from kernel-space then it should be 0

// the thread pointer should be at the base of the sscratch location if it is not 0

.align 4
trap_entry:
	csrrw sp, sscratch, sp
	csrrw t0, sscratch, t0

	// sp <- sscratch
	// t0 <- old sp
	// sscratch <- old t0
	bnez sp, save_regs

from_smode:
	// sp == 0
	// t0 <- old sp (kernel stack pointer)
	// sscratch <- old t0

	// someone could be stupid and have made kernel's sp not 8-byte aligned -> align down to 16-bytes
	and sp, t0, -16

save_regs:
	// sp != 0 - valid kernel stack pointer
	// t0 <- old sp
	// sscratch <- old t0

	add sp, sp, -FRAME_SIZE
	// save original sp, restore original t0 in sscratch and restore sscratch invariant
	sd t0, 2*8(sp)
	csrrw t0, sscratch, zero

	// Save everything, but sp (x2)
	.irp i, 0,1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
		sd x\i, \i*8(sp)
	.endr

	csrr t0, sepc
	csrr t1, sstatus

	sd t0, 32*8(sp)
	sd t1, 33*8(sp)

	andi t1, t1, SSTATUS_SPP
	bnez t1, run_handler

.option push
.option norelax
	lla gp, __global_pointer$
.option pop

	ld tp, FRAME_SIZE(sp)

run_handler:
	mv a0, sp
	call trap_handler_trampoline

kernel_trap_restore:
	ld t0, 32*8(sp)
	ld t1, 33*8(sp)

	// disable interrupts and restore sstatus and sepc
	and t2, t1, ~SSTATUS_SIE
	csrw sstatus, t2
	csrw sepc, t0

	and t2, t1, SSTATUS_SPP
	bnez t2, load_regs

restore_umode:
	sd tp, FRAME_SIZE(sp)

	add t0, sp, FRAME_SIZE
	csrw sscratch, t0

load_regs:
	.irp i, 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
		ld x\i, \i*8(sp)
	.endr

	ld sp, 2*8(sp)
	sret

trap_restore_with_cleanup:
	mv sp, a0
	beqz a2, kernel_trap_restore
	lla ra, kernel_trap_restore
	mv a0, a1
	jr a2

trap_utils_jump_with_stack:
	mv sp, a0
	mv a0, a1
	mv ra, zero
	jr a2
