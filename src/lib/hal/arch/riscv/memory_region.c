#include <hal/memory_regions.h>
#include <stdbigos/error.h>

typedef struct {
	u32 idx;
	bool is_in_resmem;
} riscv_hal_res_mem_iter_t;
static_assert(sizeof(riscv_hal_res_mem_iter_t) <= sizeof(hal_reserved_memory_iterator_t));

error_t hal_get_reserved_regions_iterator(hal_reserved_memory_iterator_t* iterOUT) {
	(void)iterOUT;
	return ERR_NOT_INITIALIZED;
}

error_t hal_get_next_reserved_region(hal_reserved_memory_iterator_t* iter, memory_area_t* areaOUT) {
	(void)iter;
	(void)areaOUT;
	return ERR_NOT_INITIALIZED;
}

typedef struct {
	u32 idx;
} riscv_hal_mem_iter_t;
static_assert(sizeof(riscv_hal_mem_iter_t) <= sizeof(hal_memory_iterator_t));

error_t hal_get_memory_regions_iterator(hal_memory_iterator_t* iterOUT) {
	(void)iterOUT;
	return ERR_NOT_INITIALIZED;
}

error_t hal_get_next_memory_region(hal_memory_iterator_t* iter, physical_memory_region_t* areaOUT) {
	(void)iter;
	(void)areaOUT;
	return ERR_NOT_INITIALIZED;
}
