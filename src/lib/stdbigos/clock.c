#include <stdbigos/clock.h>
#include <hal/timer.h>

static u64 g_tick_quantum;
static u64 g_next_deadline;
static u64 g_ticks;

static sbi_error_t map_hal_error(error_t err) {
	switch (err) {
	case ERR_NONE: return SBI_SUCCESS;
	case ERR_BAD_ARG: return SBI_ERR_INVALID_PARAM;
	case ERR_NOT_INITIALIZED:
	case ERR_NOT_VALID: return SBI_ERR_INVALID_STATE;
	case ERR_NOT_IMPLEMENTED: return SBI_ERR_NOT_SUPPORTED;
	default: return SBI_ERR_FAILED;
	}
}

static struct sbiret make_error(error_t error) {
	return (struct sbiret){.error = map_hal_error(error), .value = 0};
}

static struct sbiret clock_program_timer(u64 deadline) {
	error_t err = hal_timer_set_deadline(deadline);
	if (err != ERR_NONE) {
		return make_error(err);
	}

	return (struct sbiret){.error = SBI_SUCCESS, .value = 0};
}

u64 clock_now(void) {
	return hal_timer_now();
}

u64 clock_ticks(void) {
	return g_ticks;
}

struct sbiret clock_rearm(void) {
	if (g_tick_quantum == 0) {
		return make_error(ERR_NOT_INITIALIZED);
	}

	g_next_deadline = clock_now() + g_tick_quantum;
	return clock_program_timer(g_next_deadline);
}

struct sbiret clock_init(u64 tick_quantum) {
	if (tick_quantum == 0) {
		return make_error(ERR_BAD_ARG);
	}

	g_ticks = 0;
	g_tick_quantum = tick_quantum;
	return clock_rearm();
}

struct sbiret clock_on_timer_interrupt(void) {
	++g_ticks;

	error_t err = hal_timer_set_deadline(~0ull);
	if (err != ERR_NONE) {
		return make_error(err);
	}

	return (struct sbiret){.error = SBI_SUCCESS, .value = 0};
}
