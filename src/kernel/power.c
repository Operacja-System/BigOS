#include "power.h"

void shutdown() {
// TODO:
	halt();
}

void reboot() {
// TODO:
	halt();
}

void halt() {
	for(;;)
		asm volatile("wfi");
}
