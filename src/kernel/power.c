#include "power.h"

void shutdown() {
// TODO:
}

void reboot() {
// TODO:
}

void halt() {
	for(;;)
		asm volatile("wfi");
}
