#include <debug/debug_stdio.h>
#include <stdbigos/csr.h>
#include <stdbigos/error.h>
#include <stdbigos/types.h>
#include <trap/trap.h>

void handle_exc(trap_exception_type_t exc, trap_frame_t* tf) {
	[[maybe_unused]] reg_t stval = tf->stval;

	switch (exc) {
	case TRAP_EXC_ENV_CALL_U:
		intr_enable();
		tf->sepc += 4; // skip ECALL instruction

		DEBUG_PRINTF("syscall: got 0x%lx\n", tf->a0);
		tf->a0 = 0xeed;

		intr_disable();
		return;
	default:
		// we've got reserved exception
		DEBUG_PRINTF("exception: got unknown number %lu\n", (u64)exc);
		return;
	}
}

void my_trap_handler(trap_frame_t* tf) {
	reg_t scause = tf->scause;
	if (trap_is_interrupt(scause)) {
		DEBUG_PRINTF("got interrupt: %lu\n", (u64)trap_get_interrupt_code(scause));
	} else {
		handle_exc(trap_get_exception_code(scause), tf);
	}
}

static u8 g_kernel_mode_stack[4096] __attribute__((aligned(4096)));
static u8 g_kernel_mode_stack2[4096] __attribute__((aligned(4096)));
static u8 g_user_mode_stack[4096] __attribute__((aligned(4096)));

void user_fn() {
	while (1) {
		DEBUG_PRINTF("from user mode\n");

		register reg_t a0 __asm__("a0") = 0xdeadbeef;
		__asm__ volatile("ecall" : "+r"(a0));
		DEBUG_PRINTF("syscall returned: 0x%lx\n", a0);

		// try to do something bad with sp, so we know kernel will not break
		__asm__ volatile("mv t0, sp\n\t"
		                 "mv sp, zero\n\t"
		                 "ecall\n\t"
		                 "mv sp, t0"
		                 : "+r"(a0)
		                 :
		                 : "t0");
	}
}

typedef struct {
	u32 hartid;
	const void* fdt;
} kernel_state_t;

static inline void* get_sp() {
	void* sp;
	__asm__("mv %0, sp" : "=r"(sp));
	return sp;
}

void kernel_continuation(void* usr) {
	// copy the state from the old stack
	kernel_state_t state = *(kernel_state_t*)usr;
	DEBUG_PRINTF("on new stack: hartid %d, fdt %p, sp %p\n", state.hartid, state.fdt, get_sp());

	// from now on we can cleanup the old stack

	// now jump to user mode (also reenter our stack)
	void* user_stack_top = &g_user_mode_stack[sizeof(g_user_mode_stack)];
	void* kernel_stack_top = &g_kernel_mode_stack2[sizeof(g_kernel_mode_stack2)];
	trap_frame_t tf = {0};

	tf.sp = (reg_t)user_stack_top;
	tf.sepc = (reg_t)user_fn;   // setup user entry point
	tf.sstatus = CSR_READ_RELAXED(sstatus);
	tf.sstatus &= ~(1ull << 8); // clear previous privilege S -> sret will return to U mode

	if (trap_utils_prepare_stack_for_transition(&kernel_stack_top, &tf) != ERR_NONE) {
		DEBUG_PRINTF("failed to prepare stack for transition\n");
		return;
	}

	DEBUG_PRINTF("transition to U-mode\n");
	trap_restore_with_cleanup(kernel_stack_top, nullptr, nullptr);
}

void main([[maybe_unused]] u32 hartid, [[maybe_unused]] const void* fdt) {
	DEBUG_PRINTF("on old stack: hartid %d, fdt %p, sp %p\n", hartid, fdt, get_sp());

	if (trap_init(my_trap_handler) != ERR_NONE) {
		return;
	}

	// migrate to a different stack
	kernel_state_t state = {hartid, fdt};
	void* stack_top = &g_kernel_mode_stack[sizeof(g_kernel_mode_stack)];
	trap_utils_jump_with_stack(stack_top, &state, kernel_continuation);
}
