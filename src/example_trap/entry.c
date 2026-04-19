#include <debug/debug_stdio.h>
#include <hal/trap.h>
#include <stdbigos/error.h>
#include <stdbigos/types.h>

/**
 * Syscall handler - called when userspace makes an ecall.
 * Receives syscall arguments as reg_t (architecture-dependent).
 * Returns error code and value - error in a0, value in a1.
 */
hal_syscall_result_t syscall_handler(reg_t syscall_id, reg_t arg0, reg_t arg1, reg_t arg2, reg_t arg3, reg_t arg4,
                                     reg_t arg5) {
	(void)arg1;
	(void)arg2;
	(void)arg3;
	(void)arg4;
	(void)arg5;
	switch (syscall_id) {
	case 0: // Example syscall 0
		DEBUG_PRINTF("syscall: got 0x%lx\n", arg0);
		return (hal_syscall_result_t){.error = 0, .value = 0xeed};

	default:
		// unknown syscall
		DEBUG_PRINTF("syscall: unknown number %lu\n", (u64)syscall_id);
		return (hal_syscall_result_t){.error = 1, .value = 0};
	}
}

static u8 g_user_mode_stack[4096] __attribute__((aligned(4096)));
static u8 g_task_frame_storage[512] __attribute__((aligned(64)));

void user_fn(void) {
	while (1) {
		DEBUG_PRINTF("from user mode\n");

		register reg_t a0 __asm__("a0") = 0xbade;
		register reg_t a1 __asm__("a1") = 0xaaaa;
		register reg_t syscallid __asm__("a7") = 0;
		__asm__ volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(syscallid) : "memory");
		DEBUG_PRINTF("syscall returned: 0x%lx, 0x%lx\n", a0, a1);
	}
}

static inline void* get_sp(void) {
	void* sp;
	__asm__("mv %0, sp" : "=r"(sp));
	return sp;
}

void main([[maybe_unused]] u32 hartid, [[maybe_unused]] const void* fdt) {
	DEBUG_PRINTF("on old stack: hartid %d, fdt %p, sp %p\n", hartid, fdt, get_sp());

	// Initialize trap handling system
	if (hal_trap_init() != ERR_NONE) {
		DEBUG_PRINTF("failed to initialize trap handling\n");
		return;
	}

	if (hal_trap_register_syscall_handler(syscall_handler) != ERR_NONE) {
		DEBUG_PRINTF("failed to register syscall handler\n");
		return;
	}

	// Prepare initial userspace task and start it.
	hal_trap_frame_t* task_frame = hal_trap_frame_from_buffer(g_task_frame_storage, sizeof(g_task_frame_storage));
	if (!task_frame) {
		DEBUG_PRINTF("failed to allocate task frame\n");
		return;
	}

	void* user_stack_top = &g_user_mode_stack[sizeof(g_user_mode_stack)];
	DEBUG_PRINTF("starting user task, pc=%p, sp=%p\n", (void*)user_fn, user_stack_top);
	hal_trap_frame_init_userspace(task_frame, (uintptr_t)user_stack_top, (uintptr_t)user_fn);
	hal_trap_start_task(task_frame);

	// Should never reach here (hal_trap_start_task is noreturn)
}
