#include "debug/debug_stdio.h"
#include "power.h"
[[noreturn]] void kmain() {
	dprintf(" ____  _        ____   _____ \n");
	dprintf("|  _ \\(_)      / __ \\ / ____|\n");
	dprintf("| |_) |_  __ _| |  | | (___  \n");
	dprintf("|  _ <| |/ _` | |  | |\\___ \\ \n");
	dprintf("| |_) | | (_| | |__| |____) |\n");
	dprintf("|____/|_|\\__, |\\____/|_____/ \n");
	dprintf("          __/ |              \n");
	dprintf("         |___/               \n");
	halt();
}
