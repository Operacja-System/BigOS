#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <stdbigos/error.h>
#include <stdbigos/types.h>

/**
 * @addtogroup hal
 * @{
 */

/**
 * @brief Read current monotonic hardware timer value.
 * @return Current hardware timer tick value.
 */
u64 hal_timer_now(void);

/**
 * @brief Program the next timer interrupt deadline.
 *
 * @param deadline Absolute timer value when the interrupt should fire.
 * @retval ERR_NONE Success
 * @retval ERR_BAD_ARG Invalid deadline argument
 * @retval ERR_NOT_IMPLEMENTED Timer SBI extension unsupported
 * @retval ERR_NOT_VALID Other platform/firmware failure
 */
error_t hal_timer_arm_absolute(u64 deadline);

/**
 * @brief Program next timer interrupt relative to current time.
 *
 * @param delta Timer ticks from now
 * @retval ERR_NONE Success
 * @retval ERR_BAD_ARG Invalid delta argument
 * @retval ERR_NOT_IMPLEMENTED Timer SBI extension unsupported
 * @retval ERR_NOT_VALID Other platform/firmware failure
 */
error_t hal_timer_arm_relative(u64 delta);

/**
 * @brief Enable supervisor timer interrupts.
 */
void hal_timer_enable_interrupts(void);

/**
 * @brief Disable supervisor timer interrupts.
 */
void hal_timer_disable_interrupts(void);

/// @}

#endif // !HAL_TIMER_H
