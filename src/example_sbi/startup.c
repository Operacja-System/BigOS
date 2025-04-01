/*
   Simple C++ startup routine to setup CRT
   SPDX-License-Identifier: Unlicense

   (https://five-embeddev.com/ | http://www.shincbm.com/)

*/

#include <stdbigos/csr.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>

// Generic C function pointer.
typedef void (*function_t)(void);

// These symbols are defined by the linker script.
// See linker.lds
extern u8 __bss_start[];
extern u8 __bss_end;

extern function_t __preinit_array_start;
extern function_t __preinit_array_end;

extern function_t __init_array_start;
extern function_t __init_array_end;

extern function_t __fini_array_start;
extern function_t __fini_array_end;

extern int main(u32 hartid, const void* fdt);

[[gnu::section(".init.enter"), gnu::naked]]
void _enter(void) {
	__asm__ volatile(".option push\n\t"
					 ".option norelax\n\t"
					 "la    gp, __global_pointer$\n\t"
					 ".option pop\n\t"
					 "la    sp, __stack_start\n\t"
					 "jal   zero, _start");
}

[[gnu::section(".fini"), noreturn, gnu::noinline]]
static void _Exit(int) {
	while(1) wfi();
}

[[gnu::section(".init")]]
static void _call_constructors() {
	for(const function_t* entry = &__preinit_array_start;
		entry < &__preinit_array_end; ++entry) {
		(*entry)();
	}

	for(const function_t* entry = &__init_array_start;
		entry < &__init_array_end; ++entry) {
		(*entry)();
	}
}

[[gnu::section(".fini")]]
static void _call_destructors() {
	for(const function_t* entry = &__fini_array_start;
		entry < &__fini_array_end; ++entry) {
		(*entry)();
	}
}

[[gnu::section(".init"), noreturn, gnu::used]]
static void _start(u32 hartid, const void* fdt) {
	memset(__bss_start, 0, &__bss_end - __bss_start);
	_call_constructors();

	int rc = main(hartid, fdt);

	_call_destructors();
	_Exit(rc);
}
