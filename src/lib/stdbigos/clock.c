#include <stdbigos/clock.h>
#include <stdbigos/csr.h>

static u64 g_tick_quantum;
static u64 g_next_deadline;
static u64 g_ticks;

static struct sbiret clock_program_timer(u64 deadline) {
	struct sbiret ret = sbi_set_timer(deadline);
	if (ret.error == SBI_ERR_NOT_SUPPORTED) {
		return sbi_legacy_set_timer(deadline);
	}

	return ret;
}

static struct sbiret make_error(sbi_error_t error) {
	return (struct sbiret){.error = error, .value = 0};
}

u64 clock_now(void) {
	return CSR_READ(time);
}

u64 clock_ticks(void) {
	return g_ticks;
}

struct sbiret clock_rearm(void) {
	if (g_tick_quantum == 0) {
		return make_error(SBI_ERR_INVALID_STATE);
	}

	g_next_deadline = clock_now() + g_tick_quantum;
	return clock_program_timer(g_next_deadline);
}

struct sbiret clock_init(u64 tick_quantum) {
	if (tick_quantum == 0) {
		return make_error(SBI_ERR_INVALID_PARAM);
	}

	g_ticks = 0;
	g_tick_quantum = tick_quantum;
	return clock_rearm();
}

struct sbiret clock_on_timer_interrupt(void) {
	++g_ticks;
	return clock_rearm();
}
