#ifndef STDBIGOS_CLOCK
#define STDBIGOS_CLOCK

#include "error.h"
#include "types.h"

/// @ingroup stdbigos
/// @{
/// @ingroup clock
/// @{

u64 clock_now(void);
u64 clock_ticks(void);

error_t clock_init(u64 tick_quantum);
error_t clock_rearm(void);
error_t clock_on_timer_interrupt(void);

/// @}
/// @}

#endif // !STDBIGOS_CLOCK
