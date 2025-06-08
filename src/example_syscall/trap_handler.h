/* trap_handler.h

Header provides handling functions for traps and interrupts.

*/

#include "stdint.h"
#include "syscall.h"
#include "utils.h"

typedef struct {
    uintptr_t gpr[32];
    uintptr_t sstatus;
    uintptr_t sepc;
    uintptr_t scause;
    uintptr_t stval;
} TrapFrame;

typedef long (*syscall_t)(long, long, long, long, long, long);

#define SYS_WRITE 0

long handle_syscall(long sys_num, long arg0, long arg1, long arg2, long arg3, long arg4, long arg5);
void trap_handler(TrapFrame *frame);

long sys_write(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5);