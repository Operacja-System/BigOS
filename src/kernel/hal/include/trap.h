#ifndef HAL_TRAP_H
#define HAL_TRAP_H

#include <stdbigos/error.h>
#include <stdbigos/types.h>
#include <stddef.h>

/**
 * @addtogroup trap
 * @{
 */

/**
 * @brief Opaque trap context structure.
 *
 * Internal implementation detail - should not be accessed directly.
 */
typedef struct hal_trap_frame hal_trap_frame_t;

/**
 * @brief Timer interrupt handler callback.
 */
typedef void (*hal_timer_handler_t)(void);

/**
 * @brief Syscall return value structure.
 *
 * Holds both the error code and the return value from a syscall.
 * This allows architecture-independent representation of syscall results.
 */
typedef struct {
	reg_t error; ///< Error code (0 for success, non-zero for error)
	reg_t value; ///< Return value
} hal_syscall_result_t;

/**
 * @brief System call handler callback.
 *
 * Handles system calls (ecall/svc instructions). Parameters are
 * architecture-dependent register types.
 *
 * @param syscall_id System call number (from a7/r7)
 * @param arg0-arg5 System call arguments
 * @return Syscall result with error code and return value
 */
typedef hal_syscall_result_t (*hal_syscall_handler_t)(reg_t syscall_id, reg_t arg0, reg_t arg1, reg_t arg2, reg_t arg3,
                                                      reg_t arg4, reg_t arg5);

/**
 * @brief Returns the required size of an architecture trap frame.
 *
 * Use this together with @ref hal_trap_frame_alignment and
 * @ref hal_trap_frame_from_buffer when storing trap frames in user-provided
 * memory.
 */
size_t hal_trap_frame_size(void);

/**
 * @brief Returns required alignment of an architecture trap frame.
 */
size_t hal_trap_frame_alignment(void);

/**
 * @brief Constructs a trap frame view from a caller-provided buffer.
 *
 * The function aligns the returned pointer as needed for the current
 * architecture. If the buffer is too small after alignment adjustment,
 * returns NULL.
 *
 * @param buffer Backing memory buffer.
 * @param buffer_size Buffer size in bytes.
 * @return Pointer to trap frame in @p buffer, or NULL if requirements are not met.
 */
hal_trap_frame_t* hal_trap_frame_from_buffer(void* buffer, size_t buffer_size);

/**
 * @brief Copies the currently active trap frame to @p out_frame.
 *
 * This function is only valid while running inside trap handling code.
 *
 * @param out_frame Destination trap frame.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG if @p out_frame is NULL
 * @retval ERR_NOT_VALID if called outside trap context
 */
error_t hal_trap_frame_copy_out(hal_trap_frame_t* out_frame);

/**
 * @brief Swaps the current trap frame with @p in_out_frame.
 *
 * This function is only valid while running inside trap handling code.
 * It is intended for cooperative/preemptive task context switching.
 *
 * @param in_out_frame On input: frame to restore. On output: previous current frame.
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG if @p in_out_frame is NULL
 * @retval ERR_NOT_VALID if called outside trap context
 */
error_t hal_trap_frame_swap(hal_trap_frame_t* in_out_frame);

/**
 * @brief Initializes trap frame for entering userspace via `sret`.
 *
 * The frame is cleared and configured to enter user mode at @p user_pc with
 * stack pointer @p user_sp.
 *
 * @param frame Trap frame to initialize.
 * @param user_sp User-mode stack pointer.
 * @param user_pc User-mode program counter.
 */
void hal_trap_frame_init_userspace(hal_trap_frame_t* frame, uintptr_t user_sp, uintptr_t user_pc);

/**
 * @brief Initialize trap handling subsystem.
 *
 * Sets up trap entry point and registers trap vector. Specific handlers
 * for timers and syscalls can be registered separately using dedicated
 * registration functions.
 *
 * @retval ERR_NONE success
 */
error_t hal_trap_init(void);

/**
 * @brief Register a timer interrupt handler.
 *
 * Installs a dedicated handler for timer interrupts to be called
 * in addition to (or instead of) the generic trap handler.
 *
 * @param handler Non-null timer handler, or NULL to disable.
 *
 * @retval ERR_NONE success
 */
error_t hal_trap_register_timer_handler(hal_timer_handler_t handler);

/**
 * @brief Register a system call handler.
 *
 * Installs a dedicated handler for system calls (ecall/svc instructions).
 * The handler receives the syscall ID and arguments as architecture-dependent
 * register types, and returns both an error code and value.
 *
 * @param handler Non-null syscall handler, or NULL to disable.
 *
 * @retval ERR_NONE success
 */
error_t hal_trap_register_syscall_handler(hal_syscall_handler_t handler);

/**
 * @brief Request a deferred frame swap to occur right before returning from the handler.
 *
 * This function is only valid while running inside trap handling code.
 * The provided frame will be swapped with the current frame when exiting the trap handler,
 * allowing the restored task's register state (including results in a0/a1) to be preserved
 * without needing to explicitly skip syscall result writeback.
 *
 * This is useful for context-switching syscalls where you want to return control with the
 * restored task's registers intact, rather than writing the current syscall results.
 *
 * @param frame Frame to swap with the current frame on handler exit
 * @retval ERR_NONE on success
 * @retval ERR_BAD_ARG if @p frame is NULL
 * @retval ERR_NOT_VALID if called outside trap context
 */
error_t hal_trap_request_deferred_frame_swap(hal_trap_frame_t* frame);

/**
 * @brief Start execution of a prepared userspace task frame.
 *
 * Restores the provided trap frame and transfers control to userspace via
 * `sret`. This is a one-way operation that does not return.
 *
 * The frame should be initialized with @ref hal_trap_frame_init_userspace.
 *
 * @param frame Prepared userspace trap frame to run.
 */
[[noreturn]]
void hal_trap_start_task(const hal_trap_frame_t* frame);

/**
 * @brief Wait for the next interrupt.
 *
 * Executes a wait-for-interrupt instruction. Control returns when an
 * enabled interrupt occurs and is processed.
 */
void hal_wait_for_interrupt(void);

/// @}

#endif // !HAL_TRAP_H
