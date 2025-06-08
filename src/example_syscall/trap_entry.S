/* trap_entry.S
Trap entry for RISC-V S-mode
Saves full CPU context, calls a C trap handler, and restores context
Handles both kernel-mode and user-mode traps with proper stack switching
*/

.equ REG_SIZE, 8
.equ CTX_FRAME_SIZE, 36 * REG_SIZE

.section .text
.global trap_entry
.align 2

trap_entry:
    /* Check trap origin */
    csrrw   t0, sscratch, sp   /* t0 = original sscratch, sscratch = current sp */
    bnez    t0, .Luser_trap    /* If sscratch != 0, coming from user mode */

.Lkernel_trap:
    /* Kernel-mode trap: restore original sp and mark sscratch = 0 */
    csrw    sscratch, zero     /* Mark kernel mode for next trap */
    addi    sp, sp, -CTX_FRAME_SIZE
    j       .Lsave_context

.Luser_trap:
    /* User-mode trap: switch to kernel stack */
    mv      sp, t0             /* sp = kernel stack (from sscratch) */
    addi    sp, sp, -CTX_FRAME_SIZE

.Lsave_context:
    /* Save original SP (user or kernel) */
    sd      t0, 2*REG_SIZE(sp)
    
    /* Save general purpose registers */
    sd      ra, 1*REG_SIZE(sp)
    sd      gp, 3*REG_SIZE(sp)
    sd      tp, 4*REG_SIZE(sp)
    sd      t0, 5*REG_SIZE(sp)
    sd      t1, 6*REG_SIZE(sp)
    sd      t2, 7*REG_SIZE(sp)
    sd      s0, 8*REG_SIZE(sp)
    sd      s1, 9*REG_SIZE(sp)
    sd      a0, 10*REG_SIZE(sp)
    sd      a1, 11*REG_SIZE(sp)
    sd      a2, 12*REG_SIZE(sp)
    sd      a3, 13*REG_SIZE(sp)
    sd      a4, 14*REG_SIZE(sp)
    sd      a5, 15*REG_SIZE(sp)
    sd      a6, 16*REG_SIZE(sp)
    sd      a7, 17*REG_SIZE(sp)
    sd      s2, 18*REG_SIZE(sp)
    sd      s3, 19*REG_SIZE(sp)
    sd      s4, 20*REG_SIZE(sp)
    sd      s5, 21*REG_SIZE(sp)
    sd      s6, 22*REG_SIZE(sp)
    sd      s7, 23*REG_SIZE(sp)
    sd      s8, 24*REG_SIZE(sp)
    sd      s9, 25*REG_SIZE(sp)
    sd      s10, 26*REG_SIZE(sp)
    sd      s11, 27*REG_SIZE(sp)
    sd      t3, 28*REG_SIZE(sp)
    sd      t4, 29*REG_SIZE(sp)
    sd      t5, 30*REG_SIZE(sp)
    sd      t6, 31*REG_SIZE(sp)

    /* Save CSRs */
    csrr    t0, sstatus
    csrr    t1, sepc
    csrr    t2, scause
    csrr    t3, stval
    sd      t0, 32*REG_SIZE(sp)
    sd      t1, 33*REG_SIZE(sp)
    sd      t2, 34*REG_SIZE(sp)
    sd      t3, 35*REG_SIZE(sp)

    /* Call C trap handler */
    mv      a0, sp
    call    trap_handler

    /* Restore CSRs */
    ld      t0, 32*REG_SIZE(sp)
    ld      t1, 33*REG_SIZE(sp)
    csrw    sstatus, t0
    csrw    sepc, t1

    /* Restore general purpose registers */
    ld      ra, 1*REG_SIZE(sp)
    ld      gp, 3*REG_SIZE(sp)
    ld      tp, 4*REG_SIZE(sp)
    ld      t0, 5*REG_SIZE(sp)
    ld      t1, 6*REG_SIZE(sp)
    ld      t2, 7*REG_SIZE(sp)
    ld      s0, 8*REG_SIZE(sp)
    ld      s1, 9*REG_SIZE(sp)
    ld      a0, 10*REG_SIZE(sp)
    ld      a1, 11*REG_SIZE(sp)
    ld      a2, 12*REG_SIZE(sp)
    ld      a3, 13*REG_SIZE(sp)
    ld      a4, 14*REG_SIZE(sp)
    ld      a5, 15*REG_SIZE(sp)
    ld      a6, 16*REG_SIZE(sp)
    ld      a7, 17*REG_SIZE(sp)
    ld      s2, 18*REG_SIZE(sp)
    ld      s3, 19*REG_SIZE(sp)
    ld      s4, 20*REG_SIZE(sp)
    ld      s5, 21*REG_SIZE(sp)
    ld      s6, 22*REG_SIZE(sp)
    ld      s7, 23*REG_SIZE(sp)
    ld      s8, 24*REG_SIZE(sp)
    ld      s9, 25*REG_SIZE(sp)
    ld      s10, 26*REG_SIZE(sp)
    ld      s11, 27*REG_SIZE(sp)
    ld      t3, 28*REG_SIZE(sp)
    ld      t4, 29*REG_SIZE(sp)
    ld      t5, 30*REG_SIZE(sp)
    ld      t6, 31*REG_SIZE(sp)

    /* Determine return mode */
    andi    t0, t0, 0x100       /* Check SPP bit in saved sstatus */
    bnez    t0, .Lkernel_return /* Returning to kernel mode? */

.Luser_return:
    /* Returning to user mode: restore user stack */
    ld      sp, 2*REG_SIZE(sp)  /* Restore user stack pointer */
    /* Set sscratch to kernel stack for next trap */
    la      t1, _kernel_stack_top
    csrw    sscratch, t1
    sret

.Lkernel_return:
    /* Returning to kernel mode: restore kernel stack */
    ld      sp, 2*REG_SIZE(sp)  /* Restore kernel stack pointer */
    csrw    sscratch, zero      /* Mark kernel mode for next trap */
    sret