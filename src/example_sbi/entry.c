#include <stdbigos/clock.h>
#include <hal/timer.h>
#include <hal/trap.h>
#include <stdbigos/sbi.h>
#include <stdbigos/types.h>
#include <stdbool.h>

static void sbi_puts(const char* str) {
	while (*str) sbi_debug_console_write_byte(*str++);
}

static void timer_handler(void) {
	(void)clock_on_timer_interrupt();
}

void main([[maybe_unused]] u32 hartid, [[maybe_unused]] const void* fdt) {
	if (hal_trap_init() != ERR_NONE) {
		sbi_puts("trap init failed\n");
		return;
	}

	if (hal_trap_register_timer_handler(timer_handler) != ERR_NONE) {
		sbi_puts("timer handler register failed\n");
		return;
	}

	error_t err = clock_init(50000llu);
	if (err != ERR_NONE) {
		sbi_puts("clock init failed\n");
		return;
	}

	if (hal_timer_enable_interrupts() != ERR_NONE) {
		sbi_puts("timer irq enable failed\n");
		return;
	}

	sbi_puts("clock started\n");

	while (clock_ticks() == 0) {
		hal_wait_for_interrupt();
	}

	sbi_puts("tick one-shot\n");
}
