#ifndef BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_ALLOCATOR
#define BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_ALLOCATOR

#include <memory_management/include/physical_memory/manager.h>
#include <stdbigos/error.h>
#include <stddef.h>

#include "memory_management/include/common_types.h"
#include "stdbigos/types.h"

/**
 *	@ingroup kmm
 *	@ingroup palloc
 *
 *	@brief Looks for an area of suitable size and alignment to store all metadata about the @p area that the allocator
 * needs
 *
 *	@param area The physical memory area to allocate from. Will be aligned to at least 4KiB boundary
 *	@param reserved_areas An array of reserved areas from which allocations are not allowed
 *	@param reserved_areas_count The count of reserved areas
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_BAD_ARG if @p reserved_areas is null and @p reserved_areas_count is non zero or vice versa.
 * */
[[gnu::nonnull(4)]]
error_t pmallocator_get_header(memory_area_t area, const memory_area_t* reserved_areas, u32 reserved_areas_count,
                               memory_area_t* headerOUT);

/**
 *	@ingroup kmm
 *	@ingroup palloc
 *
 *	@param area The physical memory area to allocate from. Will be aligned to at least 4KiB boundary
 *	@param header_region A memory region of size at least `pmallocator_get_header_size(@p area)` aligned to 4KiB boundary
 *	@param reserved_areas An array of reserved areas from which allocations are not allowed
 *	@param reserved_areas_count The count of reserved areas
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_BAD_ARG if @p reserved_areas is null and @p reserved_areas_count is non zero or vice versa.
 *
 *	@note The header region overlaps with area, it must be marked and not be allocated from.
 * area.
 *	@note All pointers will break upon change of address space, because this is initialized before and will be used
 * after the change, no pointers can be stored inside the `header` region.
 * */
error_t pmallocator_init_region(memory_area_t area, memory_region_t header_region, const memory_area_t* reserved_areas,
                                u32 reserved_areas_count);

/**
 *	@ingroup kmm
 *	@ingroup palloc
 *
 *	@param frame_order The number of 4KiB frames to allocate is `(1 << frame_order)`
 *	@param header_region
 *	@param addrOUT Pointer to the variable to which the return address will be written to.
 *	The return address will be aligned to the allocation size
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_NOT_VALID The requested frame order is not supported by the allocator
 *	@retval ERR_NOT_ENOUGH_MEMORY Not enough memory in the area represented by @p header_region to allocate a frame of
 * desired size
 *
 *	@note FRAME_ORDER_4KiB (order 0) must be a valid frame order.
 * */
[[gnu::nonnull]]
error_t pmallocator_allocate(frame_order_t frame_order, memory_region_t header_region, phys_addr_t* addrOUT);

/**
 *	@ingroup kmm
 *	@ingroup palloc
 *
 *	@param frame_order
 *	@param addr
 *	@param header_region
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_NOT_VALID The area represented by @p addr is already free.
 * */
error_t pmallocator_free(frame_order_t frame_order, phys_addr_t addr, memory_region_t header_region);

#endif
