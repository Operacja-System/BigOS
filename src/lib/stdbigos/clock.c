#include <hal/timer.h>
#include <hal/trap.h>
#include <stdbigos/clock.h>

static u64 g_tick_quantum;
static u64 g_next_deadline;
static u64 g_ticks;
static u64 g_next_switch_tick;

static error_t clock_program_timer(u64 deadline) {
	error_t err = hal_timer_arm_absolute(deadline);
	if (err != ERR_NONE) {
		return err;
	}

	return ERR_NONE;
}

static error_t clock_schedule_next_deadline(void) {
	if (g_tick_quantum == 0) {
		return ERR_NOT_INITIALIZED;
	}

	g_next_deadline = clock_now() + g_tick_quantum;
	return clock_program_timer(g_next_deadline);
}

static void clock_timer_irq_handler(void) {
	++g_ticks;
	(void)clock_schedule_next_deadline();
}

u64 clock_now(void) {
	return hal_timer_now();
}

u64 clock_ticks_now(void) {
	return g_ticks;
}

error_t clock_init(u64 tick_quantum) {
	if (tick_quantum == 0) {
		return ERR_BAD_ARG;
	}

	error_t err = hal_trap_register_timer_handler(clock_timer_irq_handler);
	if (err != ERR_NONE) {
		return err;
	}

	g_ticks = 0;
	g_tick_quantum = tick_quantum;
	g_next_switch_tick = 0;
	return clock_schedule_next_deadline();
}

error_t clock_set_next_switch_in(u64 ticks_from_now) {
	if (g_tick_quantum == 0) {
		return ERR_NOT_INITIALIZED;
	}

	if (ticks_from_now == 0) {
		return ERR_BAD_ARG;
	}

	g_next_switch_tick = g_ticks + ticks_from_now;
	return ERR_NONE;
}

error_t clock_next_switch_tick(u64* out_tick) {
	if (out_tick == NULL) {
		return ERR_BAD_ARG;
	}

	if (g_tick_quantum == 0) {
		return ERR_NOT_INITIALIZED;
	}

	if (g_next_switch_tick == 0) {
		return ERR_NOT_FOUND;
	}

	*out_tick = g_next_switch_tick;
	return ERR_NONE;
}

error_t clock_ticks_to_next_switch(u64* out_ticks) {
	if (out_ticks == NULL) {
		return ERR_BAD_ARG;
	}

	if (g_tick_quantum == 0) {
		return ERR_NOT_INITIALIZED;
	}

	if (g_next_switch_tick == 0) {
		return ERR_NOT_FOUND;
	}

	if (g_ticks >= g_next_switch_tick) {
		*out_ticks = 0;
		return ERR_NONE;
	}

	*out_ticks = g_next_switch_tick - g_ticks;
	return ERR_NONE;
}
