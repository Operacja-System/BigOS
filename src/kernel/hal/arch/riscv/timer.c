#ifdef __ARCH_RISCV__

	#include <hal/inlcude/hal.h>

error_t hal_init_times_interupts(hal_t hal) {
	(void)hal;
	return ERR_NOT_IMPLEMENTED;
}

error_t hal_set_timer(hal_t hal, u64 timeout /*some lable?*/) {
	(void)hal;
	(void)timeout;
	return ERR_NOT_IMPLEMENTED;
}

#endif //!__ARCH_RISCV__
