#ifndef STDBIGOS_CLOCK
#define STDBIGOS_CLOCK

#include "sbi.h"
#include "types.h"

/// @ingroup stdbigos
/// @{
/// @ingroup clock
/// @{

u64 clock_now(void);
u64 clock_ticks(void);

struct sbiret clock_init(u64 tick_quantum);
struct sbiret clock_rearm(void);
struct sbiret clock_on_timer_interrupt(void);

/// @}
/// @}

#endif // !STDBIGOS_CLOCK
