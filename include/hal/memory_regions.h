#ifndef HAL_MEMORY_REGIONS
#define HAL_MEMORY_REGIONS

#include <stdbigos/error.h>
#include <stdbigos/memory_types.h>

/**
 * This type is opaque to the caller.
 * Fields of this struct should not be modified directly.
 * */
typedef struct {
	u32 idx;
	bool in_memresblock;
} hal_reserved_iterator_t;

error_t hal_get_reserved_regions_iterator(hal_reserved_iterator_t* iterOUT);
error_t hal_get_next_reserved_region(hal_reserved_iterator_t iter, memory_area_t* areaOUT);

/**
 * This type is opaque to the caller.
 * Fields of this struct should not be modified directly.
 * */
typedef struct {
	u32 idx;
} hal_memory_iterator_t;

error_t hal_get_memory_regions_iterator(hal_memory_iterator_t* iterOUT);
error_t hal_get_next_memory_region(hal_memory_iterator_t iter, memory_area_t* areaOUT);

#endif // !HAL_MEMORY_REGIONS
