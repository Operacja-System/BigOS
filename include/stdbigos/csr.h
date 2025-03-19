#ifndef _STDBIGOS_CSR_H_
#define _STDBIGOS_CSR_H_

#include "types.h"

#define CSR_OPERATION(instr, name, value)                      \
	__asm__ volatile(#instr "%i0 " #name ", %0" ::"rK"(value))

#define WRITE_CSR(name, value) CSR_OPERATION(csrw, name, value)
#define SET_CSR(name, mask)	   CSR_OPERATION(csrs, name, mask)
#define CLR_CSR(name, mask)	   CSR_OPERATION(csrc, name, mask)

#define READ_CSR(name)                                    \
	({                                                    \
		reg_t _res;                                       \
		__asm__ volatile("csrr %0, " #name : "=r"(_res)); \
		_res;                                             \
	})

#endif
