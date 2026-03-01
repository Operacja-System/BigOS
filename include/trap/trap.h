#ifndef KERNEL_TRAP_H_
#define KERNEL_TRAP_H_

#include <stdbigos/error.h>
#include <stdbigos/types.h>

typedef enum trap_interrupt_type {
	TRAP_INT_S_SOFTWARE = 1,
	TRAP_INT_S_TIMER = 5,
	TRAP_INT_S_EXTERNAL = 9,
	TRAP_INT_COUNTER_OVERFLOW = 13,
	// >=16 are for platform use
} trap_interrupt_type_t;

typedef enum trap_exception_type {
	TRAP_EXC_INSTR_ADDRESS_MISALIGNED = 0,
	TRAP_EXC_INSTR_ACCESS_FAULT = 1,
	TRAP_EXC_ILLEGAL_INSTR = 2,
	TRAP_EXC_BREAKPOINT = 3,
	TRAP_EXC_LOAD_ADDRESS_MISALIGNED = 4,
	TRAP_EXC_LOAD_ACCESS_FAULT = 5,
	TRAP_EXC_STORE_ADDRESS_MISALIGNED = 6,
	TRAP_EXC_STORE_ACCESS_FAULT = 7,
	TRAP_EXC_ENV_CALL_U = 8,
	TRAP_EXC_ENV_CALL_S = 9,
	TRAP_EXC_INSTR_PAGE_FAULT = 12,
	TRAP_EXC_LOAD_PAGE_FAULT = 13,
	TRAP_EXC_SOFTWARE_CHECK = 18,
	TRAP_EXC_HARDWARE_ERROR = 19,
	// 24-31 designated for custom use
	// 48-63 designated for custom use
	// >=64 reserved
} trap_exception_type_t;

typedef struct trap_frame {
	union {
		reg_t gpr[32];
		struct {
			reg_t zero;
			reg_t ra;
			reg_t sp;
			reg_t gp;
			reg_t tp;
			reg_t t0;
			reg_t t1;
			reg_t t2;
			reg_t s0;
			reg_t s1;
			reg_t a0;
			reg_t a1;
			reg_t a2;
			reg_t a3;
			reg_t a4;
			reg_t a5;
			reg_t a6;
			reg_t a7;
			reg_t s2;
			reg_t s3;
			reg_t s4;
			reg_t s5;
			reg_t s6;
			reg_t s7;
			reg_t s8;
			reg_t s9;
			reg_t s10;
			reg_t s11;
			reg_t t3;
			reg_t t4;
			reg_t t5;
			reg_t t6;
		};
	};
	reg_t sepc;
	reg_t sstatus;
} trap_frame_t;

typedef void (*pfn_trap_handler_t)(trap_frame_t* tf);
typedef void (*pfn_continuation_t)(void* user);

static inline bool trap_is_interrupt(reg_t cause) {
	return (ireg_t)cause < 0;
}

static inline trap_interrupt_type_t trap_get_interrupt_code(reg_t cause) {
	return (cause << 1) >> 1; // strip highest bit
}

static inline trap_exception_type_t trap_get_exception_code(reg_t cause) {
	return cause;
}

error_t trap_init(pfn_trap_handler_t handler);

[[gnu::nonnull]]
error_t trap_utils_prepare_stack_for_transition(void** stack, const trap_frame_t* tf);

[[noreturn, gnu::nonnull(1, 3)]]
void trap_utils_jump_with_stack(void* stack, void* user, pfn_continuation_t continuation);

[[noreturn, gnu::nonnull(1)]]
void trap_restore_with_cleanup(void* stack, void* user, pfn_continuation_t cleanup);

#endif
