#ifndef KERNEL_MEMORY_MANAGMENT_COMMON_MEM_TYPES
#define KERNEL_MEMORY_MANAGMENT_COMMON_MEM_TYPES

#include <stdbigos/types.h>

typedef struct {
	void* addr;
	size_t size;
} memory_region_t;

typedef struct {
	uintptr_t addr;
	size_t size;
} memory_area_t;

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
