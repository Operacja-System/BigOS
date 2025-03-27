#ifndef STDBIGOS_TRAP_H_
#define STDBIGOS_TRAP_H_

#include "types.h"

typedef enum InterruptType {
	IntSSoftware = 1,
	// 2 is reserved
	IntMSoftware = 3,
	// 4 is reserved
	IntSTimer = 5,
	// 6 is reserved
	IntMTimer = 7,
	// 8 is reserved
	IntSExternal = 9,
	// 10 is reserved
	IntMExternal = 11,
	// 12-15 reserved
	// >=16 are for platform use
} InterruptType;

typedef enum ExceptionType {
	ExcAddressMisaligned = 0,
	ExcInstrAccessFault = 1,
	ExcIllegalInstr = 2,
	ExcBreakpoint = 3,
	ExcLoadAddrMisaligned = 4,
	ExcLoadAccessFault = 5,
	ExcStoreAddressMisaligned = 6,
	ExcStoreAccessFault = 7,
	ExcEnvCallU = 8,
	ExcEnvCallS = 9,
	// 12 is reserved
	ExcEnvCallM = 11,
	ExcInstrPageFault = 12,
	ExcLoadPageFault = 13,
	// 14 is reserved
	ExcStorePageFault = 15,
	// 16-23 reserved
	// 24-31 designated for custom use
	// 32-47 reserved
	// 48-64 designated for custom use
} ExceptionType;

static inline bool is_interrupt(reg_t cause) {
	return (ireg_t)cause < 0;
}

static inline InterruptType get_interrupt_code(reg_t cause) {
	return (cause << 1) >> 1; // strip highest bit
}

static inline ExceptionType get_exception_code(reg_t cause) {
	return cause;
}

#endif
