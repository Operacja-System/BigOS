#include "allocator.h"
#include "memory_management/include/physical_memory/manager.h"
#include "stdbigos/error.h"
#include "stdbigos/math.h"
#include "stdbigos/string.h"

typedef struct {
	uintptr_t area_base_addr;
	size_t area_size;
	u64 bitmap[];
} pmallocator_header_t;

typedef struct {
	size_t first_bit;
	size_t bit_count;
} bitmap_range_t;


static size_t calculate_header_size(memory_area_t area) {
	const size_t bitmap_bits = area.size / phys_mem_get_frame_size_in_bytes(FRAME_SIZE_4KiB);
	const size_t bitmap_bytes = ALIGN_UP(bitmap_bits, 64) / 8;
	const size_t total = sizeof(pmallocator_header_t) + bitmap_bytes;
	return ALIGN_UP(total, 0x1000);
}

static bitmap_range_t addr_range_to_bitmap_range(uintptr_t range_addr, size_t range_size, uintptr_t base_addr) {

	const size_t frame_size = phys_mem_get_frame_size_in_bytes(FRAME_SIZE_4KiB);
	const uintptr_t aligned_start = ALIGN_DOWN(range_addr, frame_size);
	const uintptr_t aligned_end   = ALIGN_UP(range_addr + range_size, frame_size);

	bitmap_range_t result = {
	    .first_bit = (aligned_start - base_addr) / frame_size,
	    .bit_count = (aligned_end - aligned_start) / frame_size,
	};
	return result;
}

error_t pmallocator_get_header(memory_area_t area, const memory_area_t* reserved_areas, u32 reserved_areas_count,
                               memory_area_t* headerOUT) {

	const size_t header_size = calculate_header_size(area);

	for (uintptr_t i = area.addr; i + header_size <= area.addr + area.size; i += 0x1000) {
		memory_area_t potential_header = {
		    .addr = i,
		    .size = header_size,
		};
		bool overlaps_reserved = false;
		for (u32 j = 0; j < reserved_areas_count; ++j) {
			if (do_memory_areas_overlap(potential_header, reserved_areas[j])) {
				overlaps_reserved = true;
				break;
			}
		}
		if (!overlaps_reserved) {
			*headerOUT = potential_header;
			return ERR_NONE;
		}
	}

	return ERR_NOT_ENOUGH_MEMORY;
}

error_t pmallocator_init_region(memory_area_t area, memory_region_t header, const memory_area_t* reserved_areas,
                                u32 reserved_areas_count) {

	const size_t header_size = calculate_header_size(area);
	const size_t bitmap_size = header_size - sizeof(pmallocator_header_t);
	pmallocator_header_t *effective_header = (pmallocator_header_t*)header.addr;
	effective_header->area_base_addr = area.addr;
	effective_header->area_size = area.size;
	memset(effective_header->bitmap, 0, bitmap_size);

	for (u32 i = 0; i < reserved_areas_count; ++i) {
		memory_area_t reserved_area = reserved_areas[i];
		bitmap_range_t bitmap_range = addr_range_to_bitmap_range(reserved_area.addr, reserved_area.size, area.addr);

		for (size_t j = bitmap_range.first_bit; j < bitmap_range.first_bit + bitmap_range.bit_count; ++j) {
			effective_header->bitmap[j / 64] |= (1ULL << (j % 64));
		}
	}

	bitmap_range_t bitmap_range = addr_range_to_bitmap_range((uintptr_t)header.addr, header.size, area.addr);
	for (size_t j = bitmap_range.first_bit; j < bitmap_range.first_bit + bitmap_range.bit_count; ++j) {
		effective_header->bitmap[j / 64] |= (1ULL << (j % 64));
	}

	return ERR_NONE;
}

error_t pmallocator_allocate(u8 frame_order, memory_region_t header, phys_addr_t* addrOUT) {

	const size_t frame_size = phys_mem_get_frame_size_in_bytes(FRAME_SIZE_4KiB);
	const size_t frame_count = 1ULL << frame_order;
	pmallocator_header_t *effective_header = (pmallocator_header_t*)header.addr;

	const size_t bitmap_bits = effective_header->area_size / frame_size;

	for (size_t i = 0; i + frame_count <= bitmap_bits; i += frame_count) {

		bool all_free = true;

		for (size_t j = i; j < i + frame_count; ++j) {
			if (effective_header->bitmap[j / 64] & (1ULL << (j % 64)) ) {
				all_free = false;
				break;
			}
		}

		if (all_free) {
			for (size_t j = i; j < i + frame_count; ++j) {
				effective_header->bitmap[j / 64] |= (1ULL << (j % 64));
			}
			*addrOUT = (phys_addr_t)(effective_header->area_base_addr + (i * frame_size));
			return ERR_NONE;
		}
	}

	return ERR_NOT_ENOUGH_MEMORY;
}

error_t pmallocator_free(phys_addr_t addr, memory_region_t header) {

	const size_t frame_size = phys_mem_get_frame_size_in_bytes(FRAME_SIZE_4KiB);
	pmallocator_header_t *effective_header = (pmallocator_header_t*)header.addr;
	const uintptr_t phys_addr = (uintptr_t)addr;
	const size_t addr_bit = ( phys_addr - effective_header->area_base_addr) / frame_size;

	if (phys_addr < effective_header->area_base_addr || addr_bit >= effective_header->area_size / frame_size) {
		return ERR_OUT_OF_BOUNDS;
	}

	if (!(effective_header->bitmap[addr_bit / 64] & (1ULL << (addr_bit % 64))) ) {
		return ERR_NOT_VALID;
	}

	effective_header->bitmap[addr_bit / 64] &= ~(1ULL << (addr_bit % 64));
	return ERR_NONE;
}
