#ifndef BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_ALLOCATOR
#define BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_ALLOCATOR

#include <stdbigos/error.h>
#include <stdbigos/memory_types.h>
#include <stddef.h>

#include "stdbigos/types.h"

/**
 *
 *	@retval true @p areaOUT is valid
 *	@retval false no more reserved regions
 * */
typedef bool (*get_next_reserved_region_t)(void* user, memory_area_t* areaOUT);

/**
 *	@ingroup kmm
 *	@ingroup palloc
 *
 *	@brief Looks for an area of suitable size and alignment to store all metadata about the @p area that the allocator
 * needs
 *
 *	@param area The physical memory area to allocate from. Will be aligned to at least 4KiB boundary
 *	@param iterator A pointer to a function that returns reserved regions. Should be nullptr if there are no reserved
 *					regions.
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_OUT_OF_MEMORY Failed to find enough space for the header reagion
 * */
[[gnu::nonnull(3)]]
error_t pmallocator_get_header(memory_area_t area, get_next_reserved_region_t enumerator, void* user,
                               memory_area_t* headerOUT);

/**
 *	@ingroup kmm
 *	@ingroup palloc
 *
 *	@param area The physical memory area to allocate from. Will be aligned to at least 4KiB boundary
 *	@param header_region A memory region of size at least `pmallocator_get_header_size(@p area)` aligned to 4KiB
 * boundary
 *	@param iterator A pointer to a function that returns reserved regions. Should be nullptr if there are no reserved
 *					regions.
 *
 *	@retval ERR_NONE Success
 *
 *	@note The header region overlaps with area, it must be marked and not be allocated from. area.
 *	@note All pointers will break upon change of address space, because this is initialized before and will be used
 *		  after the change, no pointers can be stored inside the `header` region.
 * */
error_t pmallocator_init_region(memory_area_t area, memory_region_t header_region,
                                get_next_reserved_region_t enumerator, void* user);

/**
 *	@ingroup kmm
 *	@ingroup palloc
 *
 *	@param frame_order The number of 4KiB frames to allocate is `(1 << frame_order)`
 *	@param header_region
 *	@param areaOUT Pointer to the variable to which the return area will be written to.
 *	The return address will be aligned to the allocation size
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_NOT_VALID The requested frame order is not supported by the allocator
 *	@retval ERR_PHYSICAL_MEMORY_FULL Not enough memory in the area represented by @p header_region to allocate a frame
 * of desired size
 *
 *	@note FRAME_ORDER_4KiB (order 0) must be a valid frame order.
 * */
[[gnu::nonnull(3)]]
error_t pmallocator_allocate(u8 frame_order, memory_region_t header_region, memory_area_t* areaOUT);

/**
 *	@ingroup kmm
 *	@ingroup palloc
 *
 *	@param area
 *	@param header_region
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_NOT_VALID The area represented by @p area is already free.
 *	@retval ERR_PHYSICAL_MEMORY_FULL Not enough memory in the area represented by @p header_region to allocate a frame
 * of desired size
 *
 * */
error_t pmallocator_free(memory_area_t area, memory_region_t header_region);

#endif
