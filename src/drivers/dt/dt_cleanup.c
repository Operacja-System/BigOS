#include <drivers/dt/dt.h>
#include <drivers/dt/dt_alloc.h>

void dt_cleanup(void) {
	dt_arena_reset();

	root_node = ((void*)0);
}
