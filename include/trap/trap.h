/**
 * @file trap.h
 * @brief Supervisor-mode trap dispatch and context transition helpers.
 */

#ifndef KERNEL_TRAP_H_
#define KERNEL_TRAP_H_

#include <stdbigos/error.h>
#include <stdbigos/types.h>

/**
 * @ingroup trap
 * @brief Encoded interrupt cause values from the RISC-V `scause` CSR.
 */
typedef enum trap_interrupt_type {
	TRAP_INT_S_SOFTWARE = 1,
	TRAP_INT_S_TIMER = 5,
	TRAP_INT_S_EXTERNAL = 9,
	TRAP_INT_COUNTER_OVERFLOW = 13,
	// >=16 are for platform use
} trap_interrupt_type_t;

/** @brief Encoded exception cause values from the RISC-V `scause` CSR. */
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
	TRAP_EXC_STORE_PAGE_FAULT = 15,
	TRAP_EXC_SOFTWARE_CHECK = 18,
	TRAP_EXC_HARDWARE_ERROR = 19,
	// 24-31 designated for custom use
	// 48-63 designated for custom use
	// >=64 reserved
} trap_exception_type_t;

/**
 * @ingroup trap
 * @brief Saved trap context.
 *
 * Structure representing the saved context of a trap, including general-purpose registers and relevant CSRs.
 * It is also used for state transitions (e.g from S-mode to U-mode) by preparing the structure on the
 * target stack and performing a trap return (see `trap_utils_prepare_stack_for_transition` and
 * `trap_restore_with_cleanup`).
 */
typedef struct trap_frame {
	union {
		/** @brief General-purpose registers */
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
	/** @brief value of `sepc` CSR */
	reg_t sepc;
	/** @brief value of `sstatus` CSR */
	reg_t sstatus;
} trap_frame_t;

/**
 * @ingroup trap
 * @brief Trap handler callback executed by the trap trampoline.
 */
typedef void (*pfn_trap_handler_t)(trap_frame_t* tf);
/**
 * @ingroup trap
 * @brief Continuation callback used by stack transition helpers.
 */
typedef void (*pfn_continuation_t)(void* user);

/**
 * @ingroup trap
 * @brief Returns `true` if a trap cause corresponds to an interrupt.
 * @param cause Raw `scause` value.
 * @return `true` if the cause is an interrupt, `false` if it is an exception.
 */
bool trap_is_interrupt(reg_t cause);

/**
 * @ingroup trap
 * @brief Extracts interrupt code from `scause` by clearing the interrupt bit.
 * @param cause Raw `scause` value.
 * @return Interrupt code.
 */
trap_interrupt_type_t trap_get_interrupt_code(reg_t cause);

/**
 * @ingroup trap
 * @brief Returns exception code from `scause`.
 * @param cause Raw `scause` value.
 * @return Exception code.
 */
trap_exception_type_t trap_get_exception_code(reg_t cause);

/**
 * @ingroup trap
 * @brief Installs trap entry and sets kernel trap callback.
 *
 * @param handler Non-null C trap handler.
 *
 * @retval ERR_NONE - success
 */
[[gnu::nonnull]]
error_t trap_init(pfn_trap_handler_t handler);

/**
 * @ingroup trap
 * @brief Prepares a stack so trap restore can continue from a supplied frame.
 *
 * @param stack In/out top of the kernel stack pointer after the transition. Updated to point to the new top after
 * pushing the trap frame.
 * @param tf Trap frame copied onto the target stack.
 *
 * @retval ERR_NONE - success
 */
[[gnu::nonnull]]
error_t trap_utils_prepare_stack_for_transition(void** stack, const trap_frame_t* tf);

/**
 * @ingroup trap
 * @brief Switches to `stack` and transfers control to `continuation(user)`.
 *
 * This is a helper routine for performing stack/context transitions, e.g. when
 * migrating from the boot stack to a per-hart kernel stack. The `continuation`
 * callback is executed after the stack switch, and receives a user-provided pointer for context.
 *
 * @param stack Top of the kernel stack to switch to.
 * @param user User-provided pointer passed to the continuation callback.
 * @param continuation Non-null callback executed after the stack switch.
 */
[[noreturn, gnu::nonnull(1, 3)]]
void trap_utils_jump_with_stack(void* stack, void* user, pfn_continuation_t continuation);

/**
 * @ingroup trap
 * @brief Restores trap context from `stack`, optionally running `cleanup(user)` first on the new stack.
 *
 * This is a helper routine for performing trap return with an optional cleanup step. If `cleanup` is non-null,
 * it is executed on the new stack before performing the trap return to user mode.
 * The `user` pointer is passed to the cleanup callback for context.
 *
 * @param stack Top of the kernel stack containing the trap frame to restore.
 * @param user User-provided pointer passed to the cleanup callback.
 * @param cleanup Optional callback executed before trap return. If null, the trap return is performed immediately
 * without cleanup.
 */
[[noreturn, gnu::nonnull(1)]]
void trap_restore_with_cleanup(void* stack, void* user, pfn_continuation_t cleanup);

#endif
