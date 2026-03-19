#include <stdbigos/csr.h>
#include <stdbigos/error.h>
#include <stdbigos/math.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stdint.h>
#include <trap/trap.h>

extern void trap_entry();

static pfn_trap_handler_t g_trap_handler = nullptr;

void trap_handler_trampoline(trap_frame_t* tf) {
	// disable:
	// - floating point extensions
	// - vector extensions
	// - MXR - memory executable readable
	// - SUM - supervisor read userspace
	// - SDT - double trap detection
	CSR_CLEAR(sstatus, (CSR_SSTATUS_VS_MASK << CSR_SSTATUS_VS_OFFSET) | (CSR_SSTATUS_FS_MASK << CSR_SSTATUS_FS_OFFSET) |
	                       CSR_SSTATUS_MXR | CSR_SSTATUS_SUM | CSR_SSTATUS_SDT | 0);
	if (g_trap_handler)
		g_trap_handler(tf);
}

error_t trap_init(pfn_trap_handler_t handler) {
	g_trap_handler = handler;
	CSR_WRITE(sscratch, 0);
	CSR_WRITE(stvec, trap_entry);
	return ERR_NONE;
}

error_t trap_utils_prepare_stack_for_transition(void** stack, const trap_frame_t* tf) {
	u8* sp = (u8*)ALIGN_DOWN((uintptr_t)*stack, 16);
	sp = sp - ALIGN_UP(16 + sizeof(*tf), 16); // leave space for tp and trap frame
	memcpy(sp, tf, sizeof(*tf));
	*stack = sp;

	return ERR_NONE;
}

bool trap_is_interrupt(reg_t cause) {
	return (ireg_t)cause < 0;
}

trap_interrupt_type_t trap_get_interrupt_code(reg_t cause) {
	return (cause << 1) >> 1; // strip highest bit
}

trap_exception_type_t trap_get_exception_code(reg_t cause) {
	return cause;
}
