#include "trap.h"

#include <debug/debug_stdio.h>
#include <hal/trap.h>
#include <stdbigos/error.h>
#include <stdbigos/math.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stdint.h>

#include "csr.h"
#include "csr_vals.h"

extern void hal_riscv_trap_entry();
extern void hal_riscv_trap_restore(void*);

// Global registered handlers
static hal_timer_handler_t g_timer_handler = nullptr;
static hal_syscall_handler_t g_syscall_handler = nullptr;
static riscv_trap_frame_t* g_active_trap_frame = nullptr;
static riscv_trap_frame_t* g_deferred_swap_frame = nullptr;

size_t hal_trap_frame_size(void) {
	return sizeof(riscv_trap_frame_t);
}

size_t hal_trap_frame_alignment(void) {
	return alignof(riscv_trap_frame_t);
}

hal_trap_frame_t* hal_trap_frame_from_buffer(void* buffer, size_t buffer_size) {
	if (!buffer) {
		return nullptr;
	}

	const size_t alignment = hal_trap_frame_alignment();
	const uintptr_t start = (uintptr_t)buffer;
	const uintptr_t aligned = ALIGN_UP(start, alignment);
	const size_t padding = (size_t)(aligned - start);

	if (padding > buffer_size) {
		return nullptr;
	}

	if ((buffer_size - padding) < hal_trap_frame_size()) {
		return nullptr;
	}

	return (hal_trap_frame_t*)aligned;
}

error_t hal_trap_frame_copy_out(hal_trap_frame_t* out_frame) {
	if (!out_frame) {
		return ERR_BAD_ARG;
	}

	if (!g_active_trap_frame) {
		return ERR_NOT_VALID;
	}

	memcpy(out_frame, g_active_trap_frame, sizeof(*g_active_trap_frame));
	return ERR_NONE;
}

error_t hal_trap_frame_swap(hal_trap_frame_t* in_out_frame) {
	if (!in_out_frame) {
		return ERR_BAD_ARG;
	}

	if (!g_active_trap_frame) {
		return ERR_NOT_VALID;
	}

	riscv_trap_frame_t saved = *g_active_trap_frame;
	*g_active_trap_frame = *(riscv_trap_frame_t*)in_out_frame;
	*(riscv_trap_frame_t*)in_out_frame = saved;
	return ERR_NONE;
}

void hal_trap_frame_init_userspace(hal_trap_frame_t* frame, uintptr_t user_sp, uintptr_t user_pc) {
	if (!frame) {
		return;
	}

	riscv_trap_frame_t* riscv_frame = (riscv_trap_frame_t*)frame;
	memset(riscv_frame, 0, sizeof(*riscv_frame));

	riscv_frame->sp = user_sp;
	riscv_frame->sepc = user_pc;
	riscv_frame->sstatus = 0;
}

[[noreturn]]
static void hal_riscv_trap_unhandled_interrupt(hal_riscv_trap_interrupt_t code) {
	dprintf("Unhandled interrupt code=%lu\n", (u64)code);
	while (true) {
		hal_wait_for_interrupt();
	}
}

[[noreturn]]
static void hal_riscv_trap_unhandled_exception(hal_riscv_trap_exception_t code, reg_t stval) {
	CSR_CLEAR(sstatus, CSR_SSTATUS_SIE);
	dprintf("Unhandled exception code=%lu, stval=%lu\n", (u64)code, (u64)stval);
	while (true) {
		hal_wait_for_interrupt();
	}
}

static void hal_riscv_trap_interrupt_handler(hal_riscv_trap_interrupt_t code) {
	switch (code) {
	case HAL_RISCV_TRAP_INT_S_TIMER:
		if (g_timer_handler) {
			g_timer_handler();
		}
		break;
	default: hal_riscv_trap_unhandled_interrupt(code); break;
	}
}

static void hal_riscv_trap_exception_handler(hal_riscv_trap_exception_t code, riscv_trap_frame_t* ctx) {
	switch (code) {
	case HAL_RISCV_TRAP_EXC_ENV_CALL_U:
		ctx->sepc += 4; // advance past ecall instruction

		if (g_syscall_handler) {
			// Extract syscall arguments from registers (as reg_t)
			reg_t syscall_id = ctx->a7;
			reg_t arg0 = ctx->a0;
			reg_t arg1 = ctx->a1;
			reg_t arg2 = ctx->a2;
			reg_t arg3 = ctx->a3;
			reg_t arg4 = ctx->a4;
			reg_t arg5 = ctx->a5;

			// Call handler and get result
			hal_syscall_result_t result = g_syscall_handler(syscall_id, arg0, arg1, arg2, arg3, arg4, arg5);

			// Store error in a0, value in a1 (unless deferred swap will override)
			ctx->a0 = result.error;
			ctx->a1 = result.value;
		}
		break;
	default: hal_riscv_trap_unhandled_exception(code, ctx->stval); break;
	}
}

void hal_riscv_trap_handler_trampoline(riscv_trap_frame_t* ctx) {
	bool is_top_level = g_active_trap_frame == nullptr;

	if (is_top_level) {
		g_active_trap_frame = ctx;
	}

	// disable:
	// - MXR - memory executable readable
	// - SUM - supervisor read userspace
	CSR_CLEAR(sstatus, CSR_SSTATUS_MXR | CSR_SSTATUS_SUM);

	if (hal_riscv_trap_is_interrupt(ctx->scause)) {
		hal_riscv_trap_interrupt_handler(hal_riscv_trap_get_interrupt_code(ctx->scause));
	} else {
		hal_riscv_trap_exception_handler(hal_riscv_trap_get_exception_code(ctx->scause), ctx);
	}

	if (is_top_level) {
		// Execute deferred frame swap if requested
		if (g_deferred_swap_frame) {
			riscv_trap_frame_t saved = *g_active_trap_frame;
			*g_active_trap_frame = *g_deferred_swap_frame;
			*g_deferred_swap_frame = saved;
			g_deferred_swap_frame = nullptr;
		}

		g_active_trap_frame = nullptr;
	}
}

error_t hal_trap_init(void) {
	CSR_WRITE(sscratch, 0);
	CSR_WRITE(stvec, hal_riscv_trap_entry);
	return ERR_NONE;
}

error_t hal_trap_register_timer_handler(hal_timer_handler_t handler) {
	g_timer_handler = handler;
	return ERR_NONE;
}

error_t hal_trap_register_syscall_handler(hal_syscall_handler_t handler) {
	g_syscall_handler = handler;
	return ERR_NONE;
}

error_t hal_trap_request_deferred_frame_swap(hal_trap_frame_t* frame) {
	if (!frame) {
		return ERR_BAD_ARG;
	}

	if (!g_active_trap_frame) {
		return ERR_NOT_VALID;
	}

	g_deferred_swap_frame = (riscv_trap_frame_t*)frame;
	return ERR_NONE;
}

__attribute__((noreturn)) void hal_trap_start_task(const hal_trap_frame_t* frame) {
	if (!frame) {
		// panic
		dputs("hal_trap_start_task called with null frame\n");
		while (true) {
			hal_wait_for_interrupt();
		}
	}

	riscv_trap_frame_t on_stack_frame = *(riscv_trap_frame_t*)frame;
	hal_riscv_trap_restore(&on_stack_frame);
	__builtin_unreachable();
}

void hal_wait_for_interrupt(void) {
	__asm__ volatile("wfi" ::: "memory");
}
