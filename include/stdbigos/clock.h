#ifndef STDBIGOS_CLOCK
#define STDBIGOS_CLOCK

#include "error.h"
#include "types.h"

/**
 * @file include/stdbigos/clock.h
 * @brief Public periodic clock API used by the scheduler and examples.
 *
 * The periodic clock provides a monotonic time source measured in
 * architecture-specific ticks and supports scheduling a "next switch"
 * tick for the scheduler to observe.
 *
 * @addtogroup stdbigos
 * @{
 * @addtogroup clock
 * @{
 */

/**
 * @brief Initialize the periodic clock subsystem.
 *
 * Initialize internal state and program the first timer deadline using
 * the provided tick quantum.
 *
 * @param tick_quantum Number of hardware timer ticks per software tick.
 * @return error_t E_OK on success, otherwise an error code.
 */
error_t clock_init(u64 tick_quantum);

/**
 * @brief Return the current time in hardware timer ticks.
 *
 * This returns a monotonic timestamp suitable for computing deadlines.
 *
 * @return Current time in hardware ticks.
 */
u64 clock_now(void);

/**
 * @brief Return the number of software ticks since boot.
 *
 * Software ticks are incremented by the timer IRQ handler and advance
 * at a rate of one per `tick_quantum` hardware ticks.
 *
 * @return Number of elapsed software ticks.
 */
u64 clock_ticks_now(void);

/**
 * @brief Request the scheduler be woken in a number of software ticks.
 *
 * Programs the next switch tick to be `ticks_from_now` software ticks
 * in the future. This does not block; use `clock_next_switch_tick`
 * or `clock_ticks_to_next_switch` to query the scheduled value.
 *
 * @param ticks_from_now Number of software ticks from now to schedule.
 * @return error_t E_OK on success, otherwise an error code.
 */
error_t clock_set_next_switch_in(u64 ticks_from_now);

/**
 * @brief Obtain the currently scheduled next-switch absolute tick.
 *
 * Writes the absolute software tick value at which the scheduler has
 * requested a switch into `out_tick`.
 *
 * @param out_tick Pointer to u64 to receive the absolute tick value.
 * @return error_t E_OK on success, EINVAL if `out_tick` is NULL, or
 * other error codes on failure.
 */
error_t clock_next_switch_tick(u64* out_tick);

/**
 * @brief Obtain the number of software ticks until the next switch.
 *
 * Writes into `out_ticks` the number of ticks remaining until the
 * scheduled next-switch tick. If a next-switch is not scheduled,
 * behavior is implementation-defined.
 *
 * @param out_ticks Pointer to u64 to receive remaining ticks.
 * @return error_t E_OK on success, EINVAL if `out_ticks` is NULL, or
 * other error codes on failure.
 */
error_t clock_ticks_to_next_switch(u64* out_ticks);

/** @} */ /* end group clock */
/** @} */ /* end group stdbigos */

#endif    // !STDBIGOS_CLOCK
