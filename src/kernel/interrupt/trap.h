#ifndef KERNEL_TRAP_H_
#define KERNEL_TRAP_H_

#include <stdbigos/types.h>

struct trap_frame {
	reg_t gpr[32];
	reg_t sepc;
	reg_t sstatus;
	reg_t scause;
	reg_t stval;
};

#endif
