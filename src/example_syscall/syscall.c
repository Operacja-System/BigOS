#include "syscall.h"

long riscv_syscall(long num, long arg0, long arg1, long arg2, long arg3, long arg4, long arg5) {
    register long syscall_id asm("a7") = num;
    register long r_arg0 asm("a0") = arg0;
    register long r_arg1 asm("a1") = arg1;
    register long r_arg2 asm("a2") = arg2;
    register long r_arg3 asm("a3") = arg3;
    register long r_arg4 asm("a4") = arg4;
    register long r_arg5 asm("a5") = arg5;

    asm volatile (
        "ecall"
        : "+r"(r_arg0)
        : "r"(syscall_id), "r"(r_arg1), "r"(r_arg2), "r"(r_arg3), "r"(r_arg4), "r"(r_arg5)
        : "memory"
    );
    
    return r_arg0;
}
