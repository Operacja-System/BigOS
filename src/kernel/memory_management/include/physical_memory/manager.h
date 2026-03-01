#ifndef BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_MANAGER
#define BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_MANAGER

#include <memory_management/include/common_types.h>
#include <stdbigos/address.h>
#include <stdbigos/error.h>
#include <stdbigos/types.h>

typedef __phys void* phys_addr_t;

typedef struct {
	phys_addr_t addr;
	size_t size;
} physical_memory_region_t;

static inline memory_area_t pmr_to_area(physical_memory_region_t region) {
	memory_area_t area;
	area.addr = (uintptr_t)region.addr;
	area.size = region.size;
	return area;
}

static inline physical_memory_region_t area_to_pmr(memory_area_t area) {
	physical_memory_region_t region;
	region.addr = (phys_addr_t)area.addr;
	region.size = area.size;
	return region;
}

static inline void* physical_to_effective([[maybe_unused]] __phys void* addr) {
	return nullptr;
} // TODO: this is here temporarly

// NOLINTBEGIN
// NOTE: Clang tidy has problem with k being lowercase
typedef enum : u8 {
	FRAME_SIZE_4kB = 0,
	FRAME_SIZE_8kB = 1,
	FRAME_SIZE_16kB = 2,
	FRAME_SIZE_32kB = 3,
	FRAME_SIZE_64kB = 4,
	FRAME_SIZE_128kB = 5,
	FRAME_SIZE_256kB = 6,
	FRAME_SIZE_512kB = 7,
	FRAME_SIZE_1MB = 8,
	FRAME_SIZE_2MB = 9,
	FRAME_SIZE_1GB = 18,
} frame_size_t; // NOTE: value of frame_size_t is 4kB + 2 ^ (frame_size)
// NOLINTEND

u64 phys_mem_get_frame_size_in_bytes(frame_size_t fs);

error_t phys_mem_init(physical_memory_region_t prim_reg, const memory_area_t* res_regs, u16 res_regs_count);

error_t phys_mem_add_region(physical_memory_region_t reg);

error_t phys_mem_alloc_frame(frame_size_t frame_size, phys_addr_t* addrOUT);

error_t phys_mem_free_frame(phys_addr_t addr);

#endif //! BIGOS_KERNEL_MEMORY_MANAGMENT_PHYSICAL_MEMORY_MANAGMENT
