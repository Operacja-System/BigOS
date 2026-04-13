#include <stdbigos/clock.h>
#include <stdbigos/csr.h>
#include <stdbigos/sbi.h>
#include <stdbigos/trap.h>
#include <stdbigos/types.h>
#include <stdbool.h>

static void sbi_puts(const char* str) {
	while (*str) sbi_debug_console_write_byte(*str++);
}

[[gnu::interrupt("supervisor")]]
static void int_handler() {
	reg_t cause = CSR_READ(scause);
	if (!is_interrupt(cause)) {
		return;
	}

	reg_t int_no = get_interrupt_code(cause);
	if (int_no != IntSTimer) {
		return;
	}

	(void)clock_on_timer_interrupt();
}

void main([[maybe_unused]] u32 hartid, [[maybe_unused]] const void* fdt) {
	CSR_WRITE(stvec, int_handler);

	struct sbiret ret = clock_init(50000llu);
	if (ret.error != SBI_SUCCESS) {
		sbi_puts("clock init failed\n");
		return;
	}

	CSR_SET(sie, 1lu << IntSTimer);
	CSR_SET(sstatus, 1lu << 1);

	sbi_puts("clock started\n");

	u64 last_tick = 0;
	while (true) {
		wfi();

		u64 ticks = clock_ticks();
		if (ticks != last_tick && (ticks % 100) == 0) {
			sbi_puts("tick x100\n");
		}

		last_tick = ticks;
	}
}
