/*
   Simple C++ startup routine to setup CRT
   SPDX-License-Identifier: Unlicense

   (https://five-embeddev.com/ | http://www.shincbm.com/)

*/

#include <hal/trap.h>
#include <relocations/reloc.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stdint.h>

// These symbols are defined by the linker script.
// See linker.lds

// NOLINTBEGIN(readability-identifier-naming)
extern u8 __bss_start [[gnu::weak]][];
extern u8 _end [[gnu::weak]][];

extern int main(u32 hartid, const void* fdt);

[[gnu::section(".fini"), noreturn, gnu::noinline]]
static void _Exit([[maybe_unused]] int return_code) {
	while (1) hal_wait_for_interrupt();
}

// NOLINTBEGIN(clang-analyzer-security.ArrayBound)
[[noreturn, gnu::used]]
void _start_c(u32 hartid, const void* fdt) {
	reloc_dynamic_info_t dynamic_info;
	if (!reloc_load_dynamic_info(&dynamic_info)) {
		_Exit(-1);
	}

	if (!self_relocate(&dynamic_info)) {
		_Exit(-1);
	}

	if (__bss_start) {
		memset(__bss_start, 0, (uintptr_t)_end - (uintptr_t)__bss_start);
	}

	for (uintptr_t idx = 0; idx < dynamic_info.init_arrays.preinit_count; idx++) {
		dynamic_info.init_arrays.preinit_array[idx]();
	}

	if (dynamic_info.init_arrays.init) {
		dynamic_info.init_arrays.init();
	}

	for (uintptr_t idx = 0; idx < dynamic_info.init_arrays.init_count; idx++) {
		dynamic_info.init_arrays.init_array[idx]();
	}

	int rc = main(hartid, fdt);

	for (uintptr_t idx = dynamic_info.init_arrays.fini_count; idx > 0; idx--) {
		dynamic_info.init_arrays.fini_array[idx - 1]();
	}

	if (dynamic_info.init_arrays.fini) {
		dynamic_info.init_arrays.fini();
	}

	_Exit(rc);
}
// NOLINTEND(clang-analyzer-security.ArrayBound)
// NOLINTEND(readability-identifier-naming)
