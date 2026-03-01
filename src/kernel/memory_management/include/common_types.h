#ifndef KERNEL_MEMORY_MANAGEMENT_COMMON_TYPES
#define KERNEL_MEMORY_MANAGEMENT_COMMON_TYPES

#include <stdbigos/array_sizes.h>
#include <stdbigos/types.h>

typedef struct {
	size_t size;
	void* addr __sized_by(size);
} memory_region_t;
// memory_region_t represents a contiguous range of valid, addressable memory.

typedef struct {
	size_t size;
	uintptr_t addr;
} memory_area_t;
// memory_area_t represents a range of memory, which isn't necessarily addressable.

[[nodiscard]]
memory_area_t memory_region_get_area(memory_region_t reg);

[[nodiscard]]
bool do_memory_areas_overlap(memory_area_t reg1, memory_area_t reg2);

[[nodiscard]]
memory_area_t memory_area_expand_to_alignment(memory_area_t reg, u64 align);

[[nodiscard]]
memory_area_t memory_area_shrink_to_alignment(memory_area_t reg, u64 align);

[[nodiscard]]
memory_region_t memory_region_shrink_to_alignment(memory_region_t reg, u64 align);

[[nodiscard]]
memory_area_t memory_area_expand_to_alignment_pow2(memory_area_t reg, u64 pow2);

[[nodiscard]]
memory_area_t memory_area_shrink_to_alignment_pow2(memory_area_t reg, u64 pow2);

[[nodiscard]]
memory_region_t memory_region_shrink_to_alignment_pow2(memory_region_t reg, u64 pow2);

#endif
