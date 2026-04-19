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

	struct sbiret ret = clock_init(50000llu);
	if (ret.error != SBI_SUCCESS) {
		sbi_puts("clock init failed\n");
		return;
	}

	if (hal_timer_enable_interrupts() != ERR_NONE) {
		sbi_puts("timer irq enable failed\n");
		return;
	}

	sbi_puts("clock started\n");

	u64 last_tick = 0;
	while (true) {
		hal_wait_for_interrupt();

		u64 ticks = clock_ticks();
		if (ticks != last_tick && (ticks % 100) == 0) {
			sbi_puts("tick x100\n");
		}

		last_tick = ticks;
	}
}
