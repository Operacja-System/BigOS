/* syscall.h

Header provides syscall wrapper for user.

*/

#ifndef SYSCALL_H
#define SYSCALL_H

#include "stdint.h"
#include "stddef.h"
#include "utils.h"

long riscv_syscall(long num, long arg0, long arg1, long arg2, long arg3, long arg4, long arg5);

#endif // SYSCALL_H
