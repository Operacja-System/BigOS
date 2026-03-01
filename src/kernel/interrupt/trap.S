.global kernel_trap_entry
.align 4

.equ SSTATUS_SIE, 0x00000002 // Supervisor Interrupt Enable
.equ SSTATUS_SPP, 0x00000100 // Supervisor Previous Privilege (1 = S-mode, 0 = U-mode)

// sscratch should have kernel stack pointer IIF in user-mode
// when comming from kernel-space then it should be 0

kernel_trap_entry:
	// Swap sscratch with sp IIF we come from user - mode
	csrrw sp, sscratch, sp
	bnez sp, 1f
	csrrw sp, sscratch, sp
1:
	// sp is now kernel stack pointer
	// NOTE : We cannot fault here, as if we do and we come from user - mode than
	// sscratch will have a user - mode stack pointer, breaking our invariants

	addi sp, sp, -288

	sd x1, 1*8(sp)

	// Determine the original SP to save into the trap frame.
	csrr x1, sscratch
	bnez x1, 2f

	// from kernel mode
	addi x1, sp, 288
	j 3f
2:
	// from user mode(sp in sscratch->now in t0)

	// Restore invariant
	csrw sscratch, zero
	// From now on we can fault anytime we want
3:
	// original SP in t0
	sd x1, 2*8(sp)

	// Save the rest of the registers
	.irp i, 0,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
		sd x\i, \i*8(sp)
	.endr

	csrr t0, sepc
	csrr t1, sstatus
	csrr t2, scause
	csrr t3, stval

	sd t0, 32*8(sp)
	sd t1, 33*8(sp)
	sd t2, 34*8(sp)
	sd t3, 35*8(sp)

	mv a0, sp
	call kernel_trap_handler

	// Disable interrupts to prevent corruption during restore
	csrc sstatus, SSTATUS_SIE

	ld t0, 32*8(sp)
	ld t1, 33*8(sp)
	csrw sepc, t0
	csrw sstatus, t1

	// Restore original sscratch
	andi t2, t1, SSTATUS_SPP
	bnez t2, 4f # If SPP != 0, we are returning to S-mode. Skip sscratch setup.

	// returning to user - mode -> arm sscratch with the kernel stack
	addi t2, sp, 288
	csrw sscratch, t2
4:
	// restore all registers
	.irp i, 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
		ld x\i, \i*8(sp)
	.endr

	ld sp, 2*8(sp)
	sret
