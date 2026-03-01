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
	if (g_trap_handler)
		g_trap_handler(tf);
}

error_t trap_init(pfn_trap_handler_t handler) {
	if (!handler)
		return ERR_BAD_ARG;

	CSR_WRITE(sscratch, 0);
	CSR_WRITE(stvec, trap_entry);
	g_trap_handler = handler;
	return ERR_NONE;
}

error_t trap_utils_prepare_stack_for_transition(void** stack, const trap_frame_t* tf) {
	u8* sp = (u8*)ALIGN_DOWN((uintptr_t)*stack, 16);
	sp = sp - 16 - sizeof(*tf); // leave space for tp and trap frame
	memcpy(sp, tf, sizeof(*tf));
	*stack = sp;

	return ERR_NONE;
}
