#ifdef __ARCH_RISCV__

	#include <hal/inlcude/hal.h>

error_t hal_init_irq(hal_t hal) {
	(void)hal;
	return ERR_NOT_IMPLEMENTED;
}

error_t hal_enable_irq(hal_t hal) {
	(void)hal;
	return ERR_NOT_IMPLEMENTED;
}

void hal_disable_irq(hal_t hal) {
	(void)hal;
}

#endif //!__ARCH_RISCV__
