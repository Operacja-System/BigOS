#include <hal/timer.h>
#include <hal/trap.h>
#include <stdbigos/clock.h>
#include <stdbigos/sbi.h>
#include <stdbigos/types.h>

static void sbi_puts(const char* str) {
	while (*str) sbi_debug_console_write_byte(*str++);
}

void main([[maybe_unused]] u32 hartid, [[maybe_unused]] const void* fdt) {
	if (hal_trap_init() != ERR_NONE) {
		sbi_puts("trap init failed\n");
		return;
	}

	error_t err = clock_init(50000llu);
	if (err != ERR_NONE) {
		sbi_puts("clock init failed\n");
		return;
	}

	hal_timer_enable_interrupts();

	sbi_puts("clock started\n");

	u64 last_tick = clock_ticks_now();
	while (true) {
		hal_wait_for_interrupt();

		u64 now_tick = clock_ticks_now();
		if (now_tick != last_tick) {
			if ((now_tick % 100) == 0) {
				sbi_puts("tick periodic\n");
			}
			last_tick = now_tick;
		}
	}
}
