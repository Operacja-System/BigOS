#include <debug/debug_stdio.h>
#include <stdbigos/csr.h>
#include <stdbigos/string.h>
#include <stdbigos/trap.h>
#include <stdbigos/types.h>

extern u8 bss_start;
extern u8 bss_end;

static const u64 clint_base = 0x02000000;

static volatile u64* mtime = (u64*)(clint_base + 0xBFF8);
static volatile u64* mtimecmp = (u64*)(clint_base + 0x4000);

static const u64 quant = 50000llu;

void main() {
	for(u32 i = 0;; ++i) DEBUG_PRINTF("hello OS %u\n", i);
}

[[gnu::interrupt("machine")]]
void int_handler() {
	reg_t cause = CSR_READ(mcause);
	if(is_interrupt(cause)) {
		// interrupt
		reg_t int_no = get_interrupt_code(cause);

		switch(int_no) {
		case IntMTimer:
			DEBUG_PUTS("\n\tgot timer interrupt\n");
			mtimecmp[hartid()] = *mtime + quant;
			break;
		default: DEBUG_PRINTF("\n\tunknown interrupt (%ld)\n", int_no); break;
		}

		CSR_CLEAR(mip, (reg_t)1 << int_no);
		return;
	}
}

[[noreturn]]
void start() {
	memset(&bss_start, '\0', &bss_end - &bss_start);

	// register handler
	CSR_WRITE(mtvec, int_handler);

	// request a timer interrupt
	mtimecmp[hartid()] = *mtime + quant;

	// set MIE in mstatus
	CSR_SET(mstatus, 8);

	// set TIMER in mie
	CSR_SET(mie, 1lu << IntMTimer);

	main();

	while(true) wfi();
}

[[gnu::section(".init"), gnu::naked]]
void _start() {
	__asm__("la gp, global_pointer\n\t"
			"la sp, stack_top\n\t"
			"jal ra, start");
}
