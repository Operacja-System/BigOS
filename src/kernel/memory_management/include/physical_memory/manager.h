#ifndef BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_MANAGER
#define BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_MANAGER

#include <stdbigos/address.h>
#include <stdbigos/error.h>
#include <stdbigos/memory_types.h>
#include <stdbigos/types.h>
#include <stddef.h>
#include <stdint.h>

/// @addtogroup kmm
/// @{
/// @addtogroup pmm
/// @{

/// @todo implement
static inline void* physical_to_effective([[maybe_unused]] __phys void* addr) {
	return nullptr;
}

static inline memory_region_t physical_memory_region_to_effective(physical_memory_region_t pmr) {
	memory_region_t out = {
	    .addr = physical_to_effective(pmr.addr),
	    .size = pmr.size,
	};
	return out;
}

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
 *	@brief Initializes physical memory manager
 *
 *	@param pmrs An array of allocatable regions of physical memory
 *	@param pmr_count
 *	@param reserved_areas An array of reserved areas of physical memory of count @p reserved_areas_count
 *	@param reserved_areas_count
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_REPEATED_INITIALIZATION Upon repeated initialization
 *	@retval ERR_OUT_OF_BOUND Internal memory mangament failed
 *	@retval ERR_NOT_VALID Hardware abstraction layer was not initializated beforehand
 * */
error_t phys_mem_init();

/**
 *	@retval ERR_NONE
 *	@retval ERR_OUT_OF_MEMORY The block of specified size was not able to be allocated
 * */
[[gnu::nonnull(2)]]
error_t phys_mem_alloc_frame(frame_order_t frame_size, physical_memory_region_t* regOUT);

/**
 *	@retval ERR_NONE
 *	@retval ERR_NOT_VALID The address being freed was reported as not allocated
 * */
error_t phys_mem_free_frame(physical_memory_region_t reg);

/// @}
/// @}

#endif //! BIGOS_KERNEL_MEMORY_MANAGMENT_PHYSICAL_MEMORY_MANAGMENT
