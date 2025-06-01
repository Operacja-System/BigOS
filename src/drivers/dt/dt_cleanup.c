#include <drivers/dt/dt.h>

#include "dt_alloc.h"

void dt_cleanup(void) {
	dt_arena_reset();
	dt_reset_root();
}
