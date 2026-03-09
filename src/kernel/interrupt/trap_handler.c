#include <debug/debug_stdio.h>
#include <stdbigos/csr.h>
#include <stdbigos/types.h>
#include <trap/trap.h>

static void handle_irq(trap_interrupt_type_t irq) {
	switch (irq) {
	case TRAP_INT_S_TIMER:
	case TRAP_INT_S_SOFTWARE:
	case TRAP_INT_S_EXTERNAL:
	case TRAP_INT_COUNTER_OVERFLOW: return;

	default:
		// we've got reserved signal
		DEBUG_PRINTF("interrupt: got reserved number %lu\n", (u64)irq);
		return;
	}
}

static void handle_exc(trap_exception_type_t exc, trap_frame_t* tf) {
	[[maybe_unused]] reg_t stval = CSR_READ_RELAXED(stval);

	switch (exc) {
	case TRAP_EXC_ENV_CALL_U:
		intr_enable();
		tf->sepc += 4; // skip ECALL instruction

		// do something, registers are available in tf

		intr_disable();
		return;
	default:
		// we've got reserved exception
		DEBUG_PRINTF("exception: got reserved number %lu\n", (u64)exc);
		return;
	}
}

void kernel_trap_handler(trap_frame_t* tf) {
	reg_t scause = CSR_READ_RELAXED(scause);
	if (trap_is_interrupt(scause)) {
		handle_irq(trap_get_interrupt_code(scause));
	} else {
		handle_exc(trap_get_exception_code(scause), tf);
	}
}
