#ifndef LIBCORE_MEMORY_TYPES
#define LIBCORE_MEMORY_TYPES

#include <libcore/address.h>
#include <libcore/array_sizes.h>
#include <libcore/math.h>
#include <libcore/types.h>

/// @addtogroup libcore
/// @{
/// @addtogroup types
/// @{

///	Represents a contiguous range of valid, addressable memory.
typedef struct {
	size_t size;
	void* addr __sized_by(size);
} memory_region_t;

///	Represents a contiguous range of physical memory.
typedef struct {
	size_t size;
	__phys void* addr __sized_by(size);
} physical_memory_region_t;

///	Represents a range of memory, which isn't necessarily addressable.
typedef struct {
	size_t size;
	uintptr_t addr;
} memory_area_t;

[[nodiscard]]
static inline memory_area_t memory_region_to_area(memory_region_t reg) {
	const memory_area_t out = {
	    .addr = (uintptr_t)reg.addr,
	    .size = reg.size,
	};
	return out;
}

[[nodiscard]]
static inline memory_area_t physical_memory_region_to_area(physical_memory_region_t reg) {
	const memory_area_t out = {
	    .addr = (uintptr_t)reg.addr,
	    .size = reg.size,
	};
	return out;
}

[[nodiscard]]
static inline bool do_memory_areas_overlap(memory_area_t area1, memory_area_t area2) {
	bool cond1 = area1.addr < area2.addr + area2.size;
	bool cond2 = area2.addr < area1.addr + area1.size;
	return cond1 && cond2;
}

[[nodiscard]]
static inline bool do_memory_regions_overlap(memory_region_t reg1, memory_region_t reg2) {
	memory_area_t area1 = memory_region_to_area(reg1);
	memory_area_t area2 = memory_region_to_area(reg2);
	return do_memory_areas_overlap(area1, area2);
}

[[nodiscard]]
static inline memory_area_t memory_area_expand_to_alignment(memory_area_t area, size_t align) {
	memory_area_t aligned_reg = {
	    .addr = ALIGN_DOWN(area.addr, align),
	    .size = area.size,
	};
	aligned_reg.size = area.size + area.addr - aligned_reg.addr;
	aligned_reg.size = ALIGN_UP(aligned_reg.size, align);
	return aligned_reg;
}

[[nodiscard]]
static inline memory_area_t memory_area_shrink_to_alignment(memory_area_t area, size_t align) {
	memory_area_t aligned_reg = {
	    .addr = ALIGN_UP(area.addr, align),
	    .size = area.size,
	};
	aligned_reg.size = area.size - area.addr + aligned_reg.addr;
	aligned_reg.size = ALIGN_DOWN(aligned_reg.size, align);
	return aligned_reg;
}

[[nodiscard]]
static inline memory_region_t memory_region_shrink_to_alignment(memory_region_t region, size_t align) {
	memory_area_t area = memory_region_to_area(region);
	area = memory_area_shrink_to_alignment(area, align);
	const memory_region_t out = {
	    .addr = (void*)area.addr,
	    .size = area.size,
	};
	return out;
}

[[nodiscard]]
static inline physical_memory_region_t physical_memory_region_shrink_to_alignment(physical_memory_region_t region,
                                                                                  size_t align) {
	memory_area_t area = physical_memory_region_to_area(region);
	area = memory_area_shrink_to_alignment(area, align);
	const physical_memory_region_t out = {
	    .addr = (__phys void*)area.addr,
	    .size = area.size,
	};
	return out;
}

/// @}
/// @}

#endif // !LIBCORE_MEMORY_TYPES
