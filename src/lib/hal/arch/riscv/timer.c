#include <hal/timer.h>
#include <stdbigos/sbi.h>
#include <stdbigos/sbi_utils.h>

#include "csr.h"
#include "trap.h"

u64 hal_timer_now(void) {
	u64 now;
	__asm__ volatile("rdtime %0" : "=r"(now)::"memory");
	return now;
}

error_t hal_timer_arm_absolute(u64 deadline) {
	if (deadline == 0) {
		return ERR_BAD_ARG;
	}

	struct sbiret ret = sbi_set_timer(deadline);
	if (ret.error == SBI_ERR_NOT_SUPPORTED) {
		ret = sbi_legacy_set_timer(deadline);
	}

	return sbi_map_error(ret.error);
}

error_t hal_timer_arm_relative(u64 delta) {
	if (delta == 0) {
		return ERR_BAD_ARG;
	}

	return hal_timer_arm_absolute(hal_timer_now() + delta);
}

void hal_timer_enable_interrupts(void) {
	CSR_SET(sie, 1ul << HAL_RISCV_TRAP_INT_S_TIMER);
}

void hal_timer_disable_interrupts(void) {
	CSR_CLEAR(sie, 1ul << HAL_RISCV_TRAP_INT_S_TIMER);
}
