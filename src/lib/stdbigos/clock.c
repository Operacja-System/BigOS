#include <hal/timer.h>
#include <stdbigos/clock.h>

static u64 g_tick_quantum;
static u64 g_next_deadline;
static u64 g_ticks;

static error_t clock_program_timer(u64 deadline) {
	error_t err = hal_timer_set_deadline(deadline);
	if (err != ERR_NONE) {
		return err;
	}

	return ERR_NONE;
}

u64 clock_now(void) {
	return hal_timer_now();
}

u64 clock_ticks(void) {
	return g_ticks;
}

error_t clock_rearm(void) {
	if (g_tick_quantum == 0) {
		return ERR_NOT_INITIALIZED;
	}

	g_next_deadline = clock_now() + g_tick_quantum;
	return clock_program_timer(g_next_deadline);
}

error_t clock_init(u64 tick_quantum) {
	if (tick_quantum == 0) {
		return ERR_BAD_ARG;
	}

	g_ticks = 0;
	g_tick_quantum = tick_quantum;
	return clock_rearm();
}

error_t clock_on_timer_interrupt(void) {
	++g_ticks;

	error_t err = hal_timer_set_deadline(~0ull);
	if (err != ERR_NONE) {
		return err;
	}

	return ERR_NONE;
}
