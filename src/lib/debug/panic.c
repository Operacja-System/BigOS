#include <debug/debug_stdio.h>
#include <debug/panic.h>

void panic(const char* msg) {
	dputs("OVERCOOKED: ");
	dputs(msg);
	dputs("\n");

	while (1) {
	}
}

void error(const char* msg) {
	dputs("BURNT: ");
	dputs(msg);
	dputs("\n");
}
