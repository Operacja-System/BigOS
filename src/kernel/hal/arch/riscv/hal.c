#ifdef __ARCH_RISCV__

	#include <hal/inlcude/hal.h>

error_t hal_init(physical_memory_region_t dtb, hal_t* halOUT) {
	(void)dtb;
	(void)halOUT;
	return ERR_NOT_IMPLEMENTED;
}

error_t hal_get_reserved_regions(hal_t hal, memory_area_t* memOUT) {
	(void)hal;
	(void)memOUT;
	return ERR_NOT_IMPLEMENTED;
}

error_t hal_get_memory_regions(hal_t hal, memory_region_t* memOUT) {
	(void)hal;
	(void)memOUT;
	return ERR_NOT_IMPLEMENTED;
}

#endif //!__ARCH_RISCV__
