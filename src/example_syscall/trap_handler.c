#include "trap_handler.h"

long sys_write(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5) {
    uart_print((const char *)arg0);

    return 0;
}

long syscall_handler(long sys_num, long arg0, long arg1, long arg2, long arg3, long arg4, long arg5) {
    long result = 0;

    static void* syscall_table[] = {
        [SYS_WRITE] = sys_write
    };

    syscall_t f = syscall_table[sys_num];

    return f(arg0, arg1, arg2, arg3, arg4, arg5);
}

void trap_handler(TrapFrame *frame) {
    if (frame->scause == 9 || frame->scause == 8) {
        frame->sepc += 4;
        long syscall_id = frame->gpr[17];

        long result = syscall_handler(
            syscall_id,
            frame->gpr[10],
            frame->gpr[11],
            frame->gpr[12],
            frame->gpr[13],
            frame->gpr[14],
            frame->gpr[15]
        );

        frame->gpr[10] = result;
    } else {
        uart_print("Unhandled iterrupt!\n");
        uart_print("scause: ");
        uart_print_num(frame->scause);
        uart_print(", sepc: ");
        uart_print_num(frame->sepc);
        uart_print(", stval: ");
        uart_print_num(frame->stval);
        uart_print("\n");
        
        /* PANIC */
        while (1);
        
    }
}
