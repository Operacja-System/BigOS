.section .text
.global trap_vector
trap_vector:
    csrr a0, scause
    csrr a1, sepc
    csrr a2, stval
    j handle_trap

