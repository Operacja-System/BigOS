#ifndef BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_MANAGER
#define BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_MANAGER

#include <memory_management/include/common_types.h>
#include <stdbigos/address.h>
#include <stdbigos/error.h>
#include <stdbigos/types.h>
#include <stddef.h>
#include <stdint.h>

#include "stdbigos/array_sizes.h"

typedef __phys void* phys_addr_t;

/// @ingroup kmm
/// @ingroup kmm_manager
typedef struct {
	size_t size;
	phys_addr_t addr __sized_by(size);
} physical_memory_region_t;

/// @ingroup kmm
/// @ingroup kmm_manager
static inline memory_area_t phys_mem_reg_to_area(physical_memory_region_t region) {
	memory_area_t area;
	area.addr = (uintptr_t)region.addr;
	area.size = region.size;
	return area;
}

/// @ingroup kmm
/// @ingroup kmm_manager
static inline physical_memory_region_t area_to_phys_mem_reg(memory_area_t area) {
	physical_memory_region_t region;
	region.addr = (phys_addr_t)area.addr;
	region.size = area.size;
	return region;
}

/// @ingroup kmm
/// @ingroup kmm_manager
static inline void* physical_to_effective([[maybe_unused]] __phys void* addr) {
	return nullptr;
} // TODO: this is here temporarly

static inline memory_region_t phys_mem_reg_to_reg(physical_memory_region_t pmr) {
	memory_region_t reg = {
	    .size = pmr.size,
	    .addr = physical_to_effective(pmr.addr),
	};
	return reg;
}

/// @ingroup kmm
/// @ingroup kmm_manager
typedef enum : u8 {
	FRAME_ORDER_4KiB = 0,
	FRAME_ORDER_8KiB = 1,
	FRAME_ORDER_16KiB = 2,
	FRAME_ORDER_32KiB = 3,
	FRAME_ORDER_64KiB = 4,
	FRAME_ORDER_128KiB = 5,
	FRAME_ORDER_256KiB = 6,
	FRAME_ORDER_512KiB = 7,
	FRAME_ORDER_1MiB = 8,
	FRAME_ORDER_2MiB = 9,
	FRAME_ORDER_1GiB = 18,
} frame_order_t; // NOTE: value is the order of 4KiB frames (frame count = 1 << order)

#define PAGE_SIZE 0x1000UL

/// @ingroup kmm
/// @ingroup kmm_manager
u64 phys_mem_get_frame_size_in_bytes(frame_order_t fs);

/**
 *	@ingroup kmm
 *	@ingroup kmm_manager
 *	@brief Initializes physical memory manager
 *
 *	@param pmrs An array of allocatable regions of physical memory
 *	@param pmr_count
 *	@param reserved_areas An array of reserved areas of physical memory of count @p reserved_areas_count
 *	@param reserved_areas_count
 *
 *	@retval ERR_NONE
 *	@retval ERR_REPEATED_INITIALIZATION
 *	@retval ERR_OUT_OF_BOUND Internal memory mangament failed
 *	@retval ERR_BAD_ARG @p pmr_count is zero
 * */
[[gnu::nonnull]]
error_t phys_mem_init(const physical_memory_region_t* pmrs, size_t pmr_count, const memory_area_t* reserved_areas,
                      size_t reserved_areas_count);

/**
 *	@ingroup kmm
 *	@ingroup kmm_manager
 *	@retval ERR_NONE
 *	@retval ERR_OUT_OF_MEMORY The block of specified size was not able to be allocated
 * */
[[gnu::nonnull]]
error_t phys_mem_alloc_frame(frame_order_t frame_size, phys_addr_t* addrOUT);

/**
 *	@ingroup kmm
 *	@ingroup kmm_manager
 *	@retval ERR_NONE
 *	@retval ERR_NOT_VALID The address being freed was reported as not allocated
 * */
error_t phys_mem_free_frame(frame_order_t frame_size, phys_addr_t addr);

#endif //! BIGOS_KERNEL_MEMORY_MANAGMENT_PHYSICAL_MEMORY_MANAGMENT
