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

static void bitmap_set(u64* bitmap, size_t bit) {
	bitmap[bit / 64] |= (1ULL << (bit % 64));
}

static void bitmap_clear(u64* bitmap, size_t bit) {
	bitmap[bit / 64] &= ~(1ULL << (bit % 64));
}

static bool bitmap_test(const u64* bitmap, size_t bit) {
	return bitmap[bit / 64] & (1ULL << (bit % 64));
}

static void bitmap_set_range(u64* bitmap, size_t start, size_t count) {
	for (size_t j = start; j < start + count; ++j) {
		bitmap_set(bitmap, j);
	}
}

static size_t calculate_header_size(memory_area_t area) {
	const size_t bitmap_bits = area.size / PAGE_SIZE;
	const size_t bitmap_bytes = ALIGN_UP(bitmap_bits, 64) / 8;
	const size_t total = sizeof(pmallocator_header_t) + bitmap_bytes;
	return ALIGN_UP(total, PAGE_SIZE);
}

static bitmap_range_t addr_range_to_bitmap_range(uintptr_t range_addr, size_t range_size, uintptr_t base_addr) {
	const uintptr_t aligned_start = ALIGN_DOWN(range_addr, PAGE_SIZE);
	const uintptr_t aligned_end = ALIGN_UP(range_addr + range_size, PAGE_SIZE);

	bitmap_range_t result = {
	    .first_bit = (aligned_start - base_addr) / PAGE_SIZE,
	    .bit_count = (aligned_end - aligned_start) / PAGE_SIZE,
	};
	return result;
}

error_t pmallocator_get_header(memory_area_t area, const memory_area_t* reserved_areas, u32 reserved_areas_count,
                               memory_area_t* headerOUT) {
	const size_t header_size = calculate_header_size(area);

	for (uintptr_t i = area.addr; i + header_size <= area.addr + area.size; i += PAGE_SIZE) {
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

error_t pmallocator_init_region(memory_area_t area, memory_region_t header_region, const memory_area_t* reserved_areas,
                                u32 reserved_areas_count) {
	pmallocator_header_t* header = header_region.addr;
	header->area_size = area.size;
	header->area_base_addr = area.addr;

	const size_t header_size = calculate_header_size(area);
	const size_t bitmap_size = header_size - sizeof(pmallocator_header_t);

	memset(header->bitmap, 0, bitmap_size);

	for (u32 i = 0; i < reserved_areas_count; ++i) {
		memory_area_t reserved_area = reserved_areas[i];
		bitmap_range_t bitmap_range = addr_range_to_bitmap_range(reserved_area.addr, reserved_area.size, area.addr);

		bitmap_set_range(header->bitmap, bitmap_range.first_bit, bitmap_range.bit_count);
	}

	bitmap_range_t bitmap_range =
	    addr_range_to_bitmap_range((uintptr_t)header_region.addr, header_region.size, area.addr);
	bitmap_set_range(header->bitmap, bitmap_range.first_bit, bitmap_range.bit_count);

	return ERR_NONE;
}

error_t pmallocator_allocate(frame_order_t frame_order, memory_region_t header_region, phys_addr_t* addrOUT) {
	pmallocator_header_t* header = header_region.addr;

	const size_t bitmap_bits = header->area_size / PAGE_SIZE;
	const size_t frame_count = 1ULL << frame_order;

	for (size_t i = 0; i + frame_count <= bitmap_bits; i += frame_count) {
		bool all_free = true;

		for (size_t j = i; j < i + frame_count; ++j) {
			if (bitmap_test(header->bitmap, j)) {
				all_free = false;
				break;
			}
		}

		if (all_free) {
			bitmap_set_range(header->bitmap, i, frame_count);
			*addrOUT = (phys_addr_t)(header->area_base_addr + (i * PAGE_SIZE));
			return ERR_NONE;
		}
	}

	return ERR_NOT_ENOUGH_MEMORY;
}

error_t pmallocator_free(frame_order_t frame_order, phys_addr_t addr, memory_region_t header_region) {
	pmallocator_header_t* header = header_region.addr;

	const size_t frame_count = 1ULL << frame_order;
	const uintptr_t phys_addr = (uintptr_t)addr;
	const size_t addr_bit = (phys_addr - header->area_base_addr) / PAGE_SIZE;
	const size_t total_pages = header->area_size / PAGE_SIZE;

	if (phys_addr < header->area_base_addr || addr_bit + frame_count > total_pages)
		return ERR_OUT_OF_BOUNDS;

	for (size_t j = addr_bit; j < addr_bit + frame_count; ++j) {
		if (!bitmap_test(header->bitmap, j))
			return ERR_NOT_VALID;
	}

	for (size_t j = addr_bit; j < addr_bit + frame_count; ++j) {
		bitmap_clear(header->bitmap, j);
	}

	return ERR_NONE;
}
