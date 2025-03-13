#include <debug/debug_stdio.h>

void example_main() {
	for(int i = 0;; ++i) DEBUG_PRINTF("hello OS %d\n", i);
}
