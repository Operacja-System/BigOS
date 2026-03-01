#include <stdbigos/csr.h>
#include <stdbigos/trap.h>

#include "trap.h"

void handle_irq(InterruptType irq) {
	switch (irq) {
	default: return;
	}
}

void handle_exc(ExceptionType exc, struct trap_frame* tf) {
	switch (exc) {
	case ExcEnvCallU:
	case ExcEnvCallS:
		intr_enable();
		tf->sepc += 4; // skip ECALL instruction

		// do something, registers are available in tf

		intr_disable();
		return;
	default: return;
	}
}

void kernel_trap_handler(struct trap_frame* tf) {
	if (is_interrupt(tf->scause)) {
		handle_irq(get_interrupt_code(tf->scause));
	} else {
		handle_exc(get_exception_code(tf->scause), tf);
	}
}
