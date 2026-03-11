#ifndef BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_ALLOCATOR
#define BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_ALLOCATOR

#include <memory_management/include/physical_memory/manager.h>
#include <stdbigos/error.h>
#include <stddef.h>

#include "memory_management/include/common_types.h"
#include "stdbigos/types.h"

/**
 *	@ingroup kmm
 *	@ingroup phys_allocator
 *
 *	@brief Looks for an area of suitable size and alignmnet to store all metadata about the @p area that the allocator
 * needs
 *
 *	@param area The phiscal memory area to allocate from. Will be aligned to at least 4KiB boundry
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
 *	@ingroup phys_allocator
 *
 *	@param area The phiscal memory area to allocate from. Will be aligned to at least 4KiB boundry
 *	@param header A memory region of size at least `pmallocator_get_header_size(@p area)` aligned to 4KiB boundry
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
error_t pmallocator_init_region(memory_area_t area, memory_region_t header, const memory_area_t* reserved_areas,
                                u32 reserved_areas_count);

/**
 *	@ingroup kmm
 *	@ingroup phys_allocator
 *
 *	@param frame_order The frame size gives as `(1 << frame_order)`
 *	@param addrOUT Pointer the the variable to which the return address will be written to.
 *	The return address will be aligned to the frame size
 *	@param header
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_NOT_VALID The requested frame size is not supported by the allocator
 *	@retval ERR_PHYSICAL_MEMORY_FULL Not enough memory in the area represented by @p header to allocate a frame of
 * desired size
 *
 *	@note 4KiB frame (or @p frame_order = 12) must be a valid frame size.
 * */
[[gnu::nonnull]]
error_t pmallocator_allocate(u8 frame_order, memory_region_t header, phys_addr_t* addrOUT);

/**
 *	@ingroup kmm
 *	@ingroup phys_allocator
 *
 *	@param addr
 *	@param header
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_NOT_VALID The area represented by @p addr is already free.
 * */
error_t pmallocator_free(phys_addr_t addr, memory_region_t header);

#endif
