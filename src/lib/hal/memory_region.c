#include <hal/memory_regions.h>
#include <stdbigos/error.h>

error_t hal_get_reserved_regions_iterator(hal_reserved_iterator_t* iterOUT) {
	(void)iterOUT;
	return ERR_NOT_INITIALIZED;
}

error_t hal_get_next_reserved_region(hal_reserved_iterator_t iter, memory_area_t* areaOUT) {
	(void)iter;
	(void)areaOUT;
	return ERR_NOT_INITIALIZED;
}

error_t hal_get_memory_regions_iterator(hal_memory_iterator_t* iterOUT) {
	(void)iterOUT;
	return ERR_NOT_INITIALIZED;
}

error_t hal_get_next_memory_region(hal_memory_iterator_t iter, memory_area_t* areaOUT) {
	(void)iter;
	(void)areaOUT;
	return ERR_NOT_INITIALIZED;
}
