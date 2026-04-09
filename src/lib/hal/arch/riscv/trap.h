/**
 * @file trap_defs.h
 * @brief Internal RISC-V trap definitions
 *
 * This header is internal to HAL and should not be included directly by user code.
 * Architecture-specific trap handling is an implementation detail of the HAL.
 */

#ifndef HAL_ARCH_RISCV_TRAP_DEFS_H
#define HAL_ARCH_RISCV_TRAP_DEFS_H

#include <hal/trap.h>
#include <stdbigos/types.h>

/**
 * @addtogroup trap
 * @{
 */

typedef struct {
	union {
		reg_t gpr[31];
		struct {
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
	reg_t stval;
	reg_t scause;
} riscv_trap_frame_t;

/// @brief Encoded interrupt cause values from the RISC-V `scause` CSR.
typedef enum hal_riscv_trap_interrupt {
	HAL_RISCV_TRAP_INT_S_SOFTWARE = 1,
	HAL_RISCV_TRAP_INT_S_TIMER = 5,
	HAL_RISCV_TRAP_INT_S_EXTERNAL = 9,
	HAL_RISCV_TRAP_INT_COUNTER_OVERFLOW = 13,
	// >=16 are for platform use
} hal_riscv_trap_interrupt_t;

/** @brief Encoded exception cause values from the RISC-V `scause` CSR. */
typedef enum hal_riscv_trap_exception {
	HAL_RISCV_TRAP_EXC_INSTR_ADDRESS_MISALIGNED = 0,
	HAL_RISCV_TRAP_EXC_INSTR_ACCESS_FAULT = 1,
	HAL_RISCV_TRAP_EXC_ILLEGAL_INSTR = 2,
	HAL_RISCV_TRAP_EXC_BREAKPOINT = 3,
	HAL_RISCV_TRAP_EXC_LOAD_ADDRESS_MISALIGNED = 4,
	HAL_RISCV_TRAP_EXC_LOAD_ACCESS_FAULT = 5,
	HAL_RISCV_TRAP_EXC_STORE_ADDRESS_MISALIGNED = 6,
	HAL_RISCV_TRAP_EXC_STORE_ADDRESS_FAULT = 7,
	HAL_RISCV_TRAP_EXC_ENV_CALL_U = 8,
	HAL_RISCV_TRAP_EXC_ENV_CALL_S = 9,
	HAL_RISCV_TRAP_EXC_INSTR_PAGE_FAULT = 12,
	HAL_RISCV_TRAP_EXC_LOAD_PAGE_FAULT = 13,
	HAL_RISCV_TRAP_EXC_STORE_PAGE_FAULT = 15,
	HAL_RISCV_TRAP_EXC_SOFTWARE_CHECK = 18,
	HAL_RISCV_TRAP_EXC_HARDWARE_ERROR = 19,
	// 24-31 designated for custom use
	// 48-63 designated for custom use
	// >=64 reserved
} hal_riscv_trap_exception_t;

/**
 * @brief Returns `true` if a trap cause corresponds to an interrupt.
 * @param cause Raw `scause` value.
 * @return `true` if the cause is an interrupt, `false` if it is an exception.
 */
static inline bool hal_riscv_trap_is_interrupt(reg_t cause) {
	return (ireg_t)cause < 0;
}

/**
 * @brief Extracts interrupt code from `scause` by clearing the interrupt bit.
 * @param cause Raw `scause` value.
 * @return Interrupt code.
 */
static inline hal_riscv_trap_interrupt_t hal_riscv_trap_get_interrupt_code(reg_t cause) {
	return (cause << 1) >> 1; // strip highest bit
}

/**
 * @brief Returns exception code from `scause`.
 * @param cause Raw `scause` value.
 * @return Exception code.
 */
static inline hal_riscv_trap_exception_t hal_riscv_trap_get_exception_code(reg_t cause) {
	return cause;
}

/// @}

#endif // !HAL_ARCH_RISCV_TRAP_DEFS_H
