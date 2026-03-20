.text

.global trap_entry
.global trap_restore_with_cleanup
.global trap_utils_jump_with_stack

.equ FRAME_SIZE, 0x120

#include <stdbigos/csr_vals.h>

// sscratch should have kernel stack pointer IIF in user-mode
// when coming from kernel-space then it should be 0

// the thread pointer should be at the base of the sscratch location if it is not 0

.align 4
trap_entry:
	csrrw sp, sscratch, sp
	csrrw a0, sscratch, a0

	// sp <- sscratch
	// a0 <- old sp
	// sscratch <- old a0
	bnez sp, .Lsave_regs

.Lfrom_smode:
	// sp == 0
	// a0 <- old sp (kernel stack pointer)
	// sscratch <- old a0

	// someone could be stupid and have made kernel's sp not 8-byte aligned -> align down to 16-bytes
	and sp, a0, -16

	// we have our hart's info in tp
	// TODO: use this to validate if we are not at the end of the stack
	//       otherwise setup panic stack and panic

.Lsave_regs:
	// sp != 0 - valid kernel stack pointer
	// a0 <- old sp
	// sscratch <- old a0

	add sp, sp, -FRAME_SIZE
	// save original sp, restore original t0 in sscratch and restore sscratch invariant
	sd a0, 1*8(sp)

	csrrw a0, sscratch, zero

	// Save everything, but sp (x2)
	.irp i, 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
		sd x\i, (\i - 1)*8(sp)
	.endr

	csrr a0, sepc
	csrr a1, sstatus
	csrr a2, stval
	csrr a3, scause
	sd a0, 31*8(sp)
	sd a1, 32*8(sp)
	sd a2, 33*8(sp)
	sd a3, 34*8(sp)

	andi a1, a1, CSR_SSTATUS_SPP
	bnez a1, .Lrun_handler

.option push
.option norelax
	lla gp, __global_pointer$
.option pop

	ld tp, FRAME_SIZE(sp)

.Lrun_handler:
	mv a0, sp
	call trap_handler_trampoline

kernel_trap_restore:
	ld a0, 31*8(sp)
	ld a1, 32*8(sp)

	// disable interrupts and restore sstatus and sepc
	and a2, a1, ~CSR_SSTATUS_SIE
	// set double trap bit
	li t0, CSR_SSTATUS_SDT
	or a2, a2, t0
	csrw sstatus, a2
	csrw sepc, a0

	and a2, a1, CSR_SSTATUS_SPP
	bnez a2, .Lload_regs

.Lrestore_umode:
	sd tp, FRAME_SIZE(sp)

	add a0, sp, FRAME_SIZE
	csrw sscratch, a0

.Lload_regs:
	.irp i, 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
		ld x\i, (\i - 1)*8(sp)
	.endr

	ld sp, 1*8(sp)
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
