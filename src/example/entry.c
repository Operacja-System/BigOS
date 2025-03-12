#include <debug/debug_stdio.h>

void example_main() {
	DEBUG_PUTS("hello OS\n");
}

void example_int_handler() {
	DEBUG_PUTS("exception triggered\n");
}
