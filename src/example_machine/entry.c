#include <debug/debug_stdio.h>
#include <relocations/reloc.h>
#include <stdbigos/csr.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stddef.h>
#include <stdint.h>
#include <trap/trap.h>

// NOLINTBEGIN(readability-identifier-naming)
extern u8 bss_start[];
extern u8 bss_end[];
// NOLINTEND(readability-identifier-naming)

static const u64 g_clint_base = 0x02000000;

static volatile u64* g_mtime = (u64*)(g_clint_base + 0xBFF8);
static volatile u64* g_mtimecmp = (u64*)(g_clint_base + 0x4000);

static const u64 g_quant = 50000llu;

#define INTERRUPT_TIMER_M 7

void main() {
	for (u32 i = 0;; ++i) {
		CSR_CLEAR(mstatus, 8); // disable interrupts
		dprintf("hello OS %u\n", i);
		CSR_SET(mstatus, 8);   // reenable interrupts
	}
}

[[gnu::interrupt("machine")]]
void int_handler() {
	reg_t cause = CSR_READ(mcause);
	if (trap_is_interrupt(cause)) {
		// interrupt
		reg_t int_no = trap_get_interrupt_code(cause);

		switch (int_no) {
		case INTERRUPT_TIMER_M:
			dputs("got timer interrupt\n");
			g_mtimecmp[hartid()] = *g_mtime + g_quant;
			break;
		default: dprintf("unknown interrupt (%ld)\n", int_no); break;
		}

		CSR_CLEAR(mip, (reg_t)1 << int_no);
		return;
	}
}

[[noreturn, gnu::used]]
void start() {
	(void)self_relocate();
	const size_t bss_sz = (uintptr_t)bss_end - (uintptr_t)bss_start;

	memset(bss_start, '\0', bss_sz);

	// register handler
	CSR_WRITE(mtvec, int_handler);

	// request a timer interrupt
	g_mtimecmp[hartid()] = *g_mtime + g_quant;

	// set MIE in mstatus
	CSR_SET(mstatus, 8);

	// set TIMER in mie
	CSR_SET(mie, 1lu << INTERRUPT_TIMER_M);

	main();

	while (true) wfi();
}
