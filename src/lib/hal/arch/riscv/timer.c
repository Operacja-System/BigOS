#include <hal/timer.h>
#include <stdbigos/sbi.h>

#include "csr.h"
#include "trap.h"

static error_t map_sbi_error(sbi_error_t err) {
	switch (err) {
	case SBI_SUCCESS:           return ERR_NONE;
	case SBI_ERR_INVALID_PARAM: return ERR_BAD_ARG;
	case SBI_ERR_NOT_SUPPORTED: return ERR_NOT_IMPLEMENTED;
	default:                    return ERR_NOT_VALID;
	}
}

u64 hal_timer_now(void) {
	return CSR_READ(time);
}

error_t hal_timer_set_deadline(u64 deadline) {
	if (deadline == 0) {
		return ERR_BAD_ARG;
	}

	struct sbiret ret = sbi_set_timer(deadline);
	if (ret.error == SBI_ERR_NOT_SUPPORTED) {
		ret = sbi_legacy_set_timer(deadline);
	}

	return map_sbi_error(ret.error);
}

error_t hal_timer_arm_relative(u64 delta) {
	if (delta == 0) {
		return ERR_BAD_ARG;
	}

	return hal_timer_set_deadline(hal_timer_now() + delta);
}

error_t hal_timer_enable_interrupts(void) {
	CSR_SET(sie, 1ul << HAL_RISCV_TRAP_INT_S_TIMER);
	return ERR_NONE;
}
