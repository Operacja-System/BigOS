#include "bootstrap_panic.h"

#include <debug/debug_stdio.h> //TODO: This should be replaced with proper printf

void bootstrap_panic(const char* file_name, u64 line, const char* msg) {
	DEBUG_PRINTF("\e[0;31mPANIC at %s:%lu - %s\n\e[0;0m", file_name, line, msg);
	for(;;);
}
