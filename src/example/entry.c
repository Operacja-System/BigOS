#include <debug/debug_stdio.h>
#include <stdbigos/csr.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>

extern u8 bss_start;
extern u8 bss_end;

static const u64 clint_base = 0x02000000;

static volatile u64* mtime = (u64*)(clint_base + 0xBFF8);
static volatile u64* mtimecmp = (u64*)(clint_base + 0x4000);

static const u64 quant = 50000llu;
static const reg_t interrupt_mask = (reg_t)1 << (__riscv_xlen - 1);

void main() {
	for(u32 i = 0;; ++i) DEBUG_PRINTF("hello OS %u\n", i);
}

[[gnu::interrupt("machine")]]
void int_handler() {
	reg_t cause = READ_CSR(mcause);
	if(cause & interrupt_mask) {
		// interrupt
		reg_t int_no = cause & ~interrupt_mask;

		switch(int_no) {
		case 4:
		case 5:
		case 6:
		case 7: // TIMER
			DEBUG_PUTS("\n\tgot timer interrupt\n");
			*mtimecmp = *mtime + quant;
			break;
		default: DEBUG_PUTS("\n\tunknown interrupt\n"); break;
		}

		CLR_CSR(mip, (reg_t)1 << int_no);
		return;
	}
}

void start() {
	memset(&bss_start, '\0', &bss_end - &bss_start);

	// register handler
	WRITE_CSR(mtvec, int_handler);

	// request a timer interrupt
	*mtimecmp = *mtime + quant;

	// set MIE in mstatus
	SET_CSR(mstatus, 8);

	// set TIMER in mie
	SET_CSR(mie, 0x80);

	main();
}

[[gnu::section(".init"), gnu::naked]]
void _start() {
	__asm__("la gp, global_pointer\n\t"
			"la sp, stack_top\n\t"
			"jal ra, start\n\t"
			"halt:\n\t"
			"wfi\n\t"
			"j halt\n\t");
}
