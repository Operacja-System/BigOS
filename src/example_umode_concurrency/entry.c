#include <debug/debug_stdio.h>
#include <hal/trap.h>
#include <stdbigos/error.h>
#include <stdbigos/types.h>

enum {
	SYSCALL_PRINT = 1,
	SYSCALL_YIELD = 2,
};

// extracted from fdt /cpus/timebase-frequency
// QEMU tells 10MHz
#define TIMEBASE_FREQUENCY 10000000ul
#define SLEEP_TIME         (u64)(TIMEBASE_FREQUENCY * 1)

static hal_trap_frame_t* g_inactive_task_frame = nullptr;

static u8 g_user_mode_stack_a[4096] __attribute__((aligned(4096)));
static u8 g_user_mode_stack_b[4096] __attribute__((aligned(4096)));

// Backing storage for both task frames.
static u8 g_active_frame_storage[512] __attribute__((aligned(64)));
static u8 g_saved_frame_storage[512] __attribute__((aligned(64)));

static inline u64 read_time() {
	u64 now;
	__asm__ volatile("rdtime %0" : "=r"(now)::"memory");
	return now;
}

static inline void user_sys_print(const char* str) {
	register reg_t a0 __asm__("a0") = (reg_t)str;
	register reg_t a7 __asm__("a7") = SYSCALL_PRINT;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a7) : "memory");
	if (a0 != 0) {
		dprintf("print syscall failed: %lu\n", (u64)a0);
	}
}

static inline void user_sys_yield() {
	register reg_t a0 __asm__("a0") = 0;
	register reg_t a7 __asm__("a7") = SYSCALL_YIELD;
	__asm__ volatile("ecall" : "=r"(a0) : "r"(a7) : "memory");
	if (a0 != 0) {
		dprintf("yield syscall failed: %lu\n", (u64)a0);
	}
}

[[noreturn]] void user_task_a() {
	while (true) {
		user_sys_print("[U-task A] running\n");
		u64 start = read_time();
		while (read_time() - start < SLEEP_TIME) user_sys_yield();
	}
}

[[noreturn]] void user_task_b() {
	while (true) {
		user_sys_print("[U-task B] running\n");
		u64 start = read_time();
		while (read_time() - start < SLEEP_TIME) user_sys_yield();
	}
}

hal_syscall_result_t syscall_handler(reg_t syscall_id, reg_t arg0, reg_t arg1, reg_t arg2, reg_t arg3, reg_t arg4,
                                     reg_t arg5) {
	(void)arg1;
	(void)arg2;
	(void)arg3;
	(void)arg4;
	(void)arg5;

	switch (syscall_id) {
	case SYSCALL_PRINT: dputs((const char*)arg0); return (hal_syscall_result_t){.error = 0, .value = 0};
	case SYSCALL_YIELD:
		if (!g_inactive_task_frame) {
			return (hal_syscall_result_t){.error = 1, .value = 0};
		}

		error_t swap_err = hal_trap_request_deferred_frame_swap(g_inactive_task_frame);
		if (swap_err != ERR_NONE) {
			return (hal_syscall_result_t){.error = (reg_t)swap_err, .value = 0};
		}
		return (hal_syscall_result_t){.error = 0, .value = 0};
	default:
		dprintf("unknown syscall id=%lu\n", (u64)syscall_id);
		return (hal_syscall_result_t){.error = 1, .value = 0};
	}
}

void main([[maybe_unused]] u32 hartid, [[maybe_unused]] const void* fdt) {
	if (hal_trap_init() != ERR_NONE) {
		dputs("trap_init failed\n");
		return;
	}

	hal_trap_frame_t* active_task_frame =
	    hal_trap_frame_from_buffer(g_active_frame_storage, sizeof(g_active_frame_storage));
	g_inactive_task_frame = hal_trap_frame_from_buffer(g_saved_frame_storage, sizeof(g_saved_frame_storage));
	if (!active_task_frame || !g_inactive_task_frame) {
		dputs("trap frame allocation failed\n");
		return;
	}

	__asm__ volatile("csrs scounteren, 0b111"); // enable all scounteren bits so that user mode can read timebase

	hal_trap_frame_init_userspace(active_task_frame, (uintptr_t)&g_user_mode_stack_a[sizeof(g_user_mode_stack_a)],
	                              (uintptr_t)user_task_a);
	hal_trap_frame_init_userspace(g_inactive_task_frame, (uintptr_t)&g_user_mode_stack_b[sizeof(g_user_mode_stack_b)],
	                              (uintptr_t)user_task_b);

	if (hal_trap_register_syscall_handler(syscall_handler) != ERR_NONE) {
		dputs("syscall handler registration failed\n");
		return;
	}

	dputs("starting U-mode concurrency example\n");
	hal_trap_start_task(active_task_frame);
}
