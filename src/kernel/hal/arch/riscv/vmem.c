#ifdef __ARCH_RISCV__

	#include <hal/inlcude/hal.h>

error_t hal_enable_virtual_memory(hal_t hal) {
	(void)hal;
	return ERR_NOT_IMPLEMENTED;
}

error_t hal_map_virtual_memory_region(hal_t hal, hardware_address_space_t* as, memory_area_t pmem, memory_area_t vmem,
                                      hal_page_selection_t ps, hal_memory_flags_t flags) {
	(void)hal;
	(void)as;
	(void)pmem;
	(void)vmem;
	(void)ps;
	(void)flags;
	return ERR_NOT_IMPLEMENTED;
}

void hal_flush_tlb(hal_t hal) {
	(void)hal;
}

error_t hal_flush_tlb_address_space(hal_t hal, hardware_address_space_t* as) {
	(void)hal;
	(void)as;
	return ERR_NOT_IMPLEMENTED;
}

#endif //!__ARCH_RISCV__
