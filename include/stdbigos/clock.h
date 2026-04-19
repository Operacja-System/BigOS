#ifndef STDBIGOS_CLOCK
#define STDBIGOS_CLOCK

#include "error.h"
#include "types.h"

/// @ingroup stdbigos
/// @{
/// @ingroup clock
/// @{

error_t clock_init(u64 tick_quantum);

u64 clock_now(void);
u64 clock_ticks_now(void);

error_t clock_set_next_switch_in(u64 ticks_from_now);
error_t clock_next_switch_tick(u64* out_tick);
error_t clock_ticks_to_next_switch(u64* out_ticks);

/// @}
/// @}

#endif // !STDBIGOS_CLOCK
