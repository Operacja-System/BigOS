/*
   Simple C++ startup routine to setup CRT
   SPDX-License-Identifier: Unlicense

   (https://five-embeddev.com/ | http://www.shincbm.com/)

*/

#include <stdbigos/csr.h>
#include <stdbigos/types.h>
#include <stdlib.h>
#include <string.h>

extern u8 __bss_start [[gnu::weak]];
extern u8 __bss_end [[gnu::weak]];

extern void __libc_init_array();
extern void __libc_fini_array();

extern int main(u32 hartid, const void* fdt);

[[gnu::section(".init.enter"), gnu::naked]]
void _enter(void) {
	__asm__(".option push\n\t"
			".option norelax\n\t"
			"la    gp, __global_pointer$\n\t"
			".option pop");
	__asm__("la    sp, _sp");
	__asm__("j     _start");
}

void __llvm_libc_exit(int) {
	while(true) __asm__("wfi");
}

[[noreturn]]
void _start(u32 hartid, const void* fdt) {
	memset(&__bss_start, 0, &__bss_end - &__bss_start);

	__libc_init_array();
	int rc = main(hartid, fdt);
	__libc_fini_array();
	_Exit(rc);
}
