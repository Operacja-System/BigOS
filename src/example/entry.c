#include <debug/debug_stdio.h>

void example_main() {
	for (int i = 0;; ++i)
		DEBUG_PRINTF("hello OS %d\n", i);
}

void example_int_handler() {
	DEBUG_PUTS("exception triggered\n");
}
