#include <stdbigos/csr.h>
#include <stdbigos/debug_stdio.h>
#include <stdbigos/trap.h>
#include <stdbigos/types.h>
#include <stdlib.h>
#include <string.h>

static const u64 clint_base = 0x02000000;

static volatile u64* mtime = (u64*)(clint_base + 0xBFF8);
static volatile u64* mtimecmp = (u64*)(clint_base + 0x4000);

static const u64 quant = 50000llu;

extern u8 __bss_start [[gnu::weak]];
extern u8 __bss_end [[gnu::weak]];

extern u8 stack_top;
extern u8 __global_pointer$;

extern void __libc_init_array();
extern void __libc_fini_array();

[[gnu::interrupt("machine")]]
void int_handler() {
	reg_t cause = CSR_READ(mcause);
	if(is_interrupt(cause)) {
		// interrupt
		reg_t int_no = get_interrupt_code(cause);

		switch(int_no) {
		case IntMTimer:
			puts("\n\tgot timer interrupt\n");
			mtimecmp[hartid()] = *mtime + quant;
			break;
		default: printf("\n\tunknown interrupt (%ld)\n", int_no); break;
		}

		CSR_CLEAR(mip, (reg_t)1 << int_no);
		return;
	}
}

void main() {
	// register handler
	CSR_WRITE(mtvec, int_handler);

	// request a timer interrupt
	mtimecmp[hartid()] = *mtime + quant;

	// set MIE in mstatus
	CSR_SET(mstatus, 8);

	// set TIMER in mie
	CSR_SET(mie, 1lu << IntMTimer);

	for(u32 i = 0;; ++i) printf("hello OS %u\n", i);
}

[[gnu::section(".init.enter"), gnu::naked]]
void _enter(void) {
	__asm__ volatile(".option push\n\t"
					 ".option norelax\n\t"
					 "la    gp, __global_pointer$\n\t"
					 ".option pop\n\t"
					 "la    sp, stack_top\n\t"
					 "j     start");
}

void __llvm_libc_exit(int) {
	while(true) __asm__("wfi");
}

[[noreturn, gnu::used]]
void start() {
	memset(&__bss_start, 0, &__bss_end - &__bss_start);

	__libc_init_array();
	main();
	__libc_fini_array();
	_Exit(0);
}
