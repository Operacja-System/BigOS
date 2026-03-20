#include <debug/debug_stdio.h>
#include <stdbigos/csr.h>
#include <stdbigos/csr_vals.h>
#include <stdbigos/error.h>
#include <stdbigos/sbi.h>
#include <stdbigos/types.h>
#include <trap/trap.h>

enum {
	SYSCALL_PRINT = 1,
};

#define TASK_COUNT    2
#define TIMER_QUANTUM 50000ul
#define SIE_STIE      (1ul << TRAP_INT_S_TIMER)

typedef struct {
	trap_frame_t tf;
	bool initialized;
} task_ctx_t;

static task_ctx_t g_tasks[TASK_COUNT];
static u32 g_current_task = 0;

// as our handlers are non-preemptive we only need one kernel stack
// but if we wanted kernel preemption, we would need to have one stack
// per task
static u8 g_kernel_mode_stack[4096] __attribute__((aligned(4096)));

static u8 g_user_mode_stack_a[4096] __attribute__((aligned(4096)));
static u8 g_user_mode_stack_b[4096] __attribute__((aligned(4096)));

static inline u64 read_time() {
	u64 now;
	__asm__ volatile("rdtime %0" : "=r"(now));
	return now;
}

static void arm_next_timer() {
	(void)sbi_set_timer(read_time() + TIMER_QUANTUM);
}

static inline void user_sys_print(const char* str) {
	register reg_t a0 __asm__("a0") = (reg_t)str;
	register reg_t a7 __asm__("a7") = SYSCALL_PRINT;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a7) : "memory");
}

[[noreturn]] void user_task_a() {
	while (true) {
		user_sys_print("[U-task A] running\n");
		u64 start = read_time();
		while (read_time() - start < 10000);
	}
}

[[noreturn]] void user_task_b() {
	while (true) {
		user_sys_print("[U-task B] running\n");
		u64 start = read_time();
		while (read_time() - start < 10000);
	}
}

static void init_task(task_ctx_t* task, void (*entry)(), void* user_stack_top) {
	task->tf = (trap_frame_t){0};
	task->tf.sp = (reg_t)user_stack_top;
	task->tf.sepc = (reg_t)entry;
	task->tf.sstatus = CSR_READ_RELAXED(sstatus);
	task->tf.sstatus &= ~CSR_SSTATUS_SPP;
	task->tf.sstatus |= CSR_SSTATUS_SPIE;
	task->initialized = true;
}

static void handle_syscall(trap_frame_t* tf) {
	tf->sepc += 4;

	switch (tf->a7) {
	case SYSCALL_PRINT:
		dputs((const char*)tf->a0);
		tf->a0 = 0;
		break;
	default:
		dprintf("unknown syscall id=%lu\n", (u64)tf->a7);
		tf->a0 = (reg_t)-1;
		break;
	}
}

static void switch_to_next_task(trap_frame_t* tf) {
	g_tasks[g_current_task].tf = *tf;
	g_current_task = (g_current_task + 1) % TASK_COUNT;
	*tf = g_tasks[g_current_task].tf;
}

void my_trap_handler(trap_frame_t* tf) {
	const reg_t scause = tf->scause;
	if (trap_is_interrupt(scause)) {
		if (trap_get_interrupt_code(scause) == TRAP_INT_S_TIMER) {
			if (read_time() < tf->stval) {
				// Spurious timer interrupt, ignore.
				return;
			}
			arm_next_timer();
			switch_to_next_task(tf);
			dprintf("switched to task %u\n", g_current_task);
			return;
		}

		dprintf("unexpected interrupt %lu\n", (u64)trap_get_interrupt_code(scause));
		return;
	}

	switch (trap_get_exception_code(scause)) {
	case TRAP_EXC_ENV_CALL_U: handle_syscall(tf); break;
	default:
		dprintf("unexpected exception %lu stval=%p\n", (u64)trap_get_exception_code(scause), (void*)tf->stval);
		break;
	}
}

void main([[maybe_unused]] u32 hartid, [[maybe_unused]] const void* fdt) {
	if (trap_init(my_trap_handler) != ERR_NONE) {
		dputs("trap_init failed\n");
		return;
	}

	init_task(&g_tasks[0], user_task_a, &g_user_mode_stack_a[sizeof(g_user_mode_stack_a)]);
	init_task(&g_tasks[1], user_task_b, &g_user_mode_stack_b[sizeof(g_user_mode_stack_b)]);

	dputs("starting U-mode concurrency example\n");
	CSR_SET(sie, SIE_STIE);
	arm_next_timer();

	void* kernel_stack_top = &g_kernel_mode_stack[sizeof(g_kernel_mode_stack)];
	trap_frame_t tf = g_tasks[g_current_task].tf;

	if (trap_utils_prepare_stack_for_transition(&kernel_stack_top, &tf) != ERR_NONE) {
		dputs("failed to prepare transition stack\n");
		return;
	}

	trap_restore_with_cleanup(kernel_stack_top, nullptr, nullptr);
}
