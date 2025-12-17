#ifndef BIGOS_KERNEL_MEMORY_MANAGMENT_PHYSICAL_MEMORY_MANAGMENT
#define BIGOS_KERNEL_MEMORY_MANAGMENT_PHYSICAL_MEMORY_MANAGMENT

#include <memory_managment/common_mem_types.h>
#include <stdbigos/error.h>
#include <stdbigos/types.h>

typedef uintptr_t phys_addr_t;

typedef memory_area_t physical_memory_region_t;

static inline void* physical_to_effective([[maybe_unused]] phys_addr_t addr) {
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
