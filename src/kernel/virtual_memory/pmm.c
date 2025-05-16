#include "pmm.h"

#include <debug/debug_stdio.h>
#include <stdbigos/string.h>

#include "kmalloc.h"

static u64* kilo_page_frame_bitmap = nullptr;
//[0] is always null for consistant indexing
static u16* subpage_busy_counter[PAGE_SIZE_AMOUNT] = {nullptr, nullptr, nullptr, nullptr, nullptr};
static u64 page_frame_amount[PAGE_SIZE_AMOUNT] = {0};

error_t initialize_pmm(virtual_memory_scheme_t vms, u64 ram_size) {
	for(u8 i = 0; i <= vms + 3; ++i) {
		const i64 shift = 30 - 12 - i * 9; //(* sizeof(gb)) (/ 4 * sizeof(kb)) (/ sizeof(page_size[i]))
		const u64 size = ((shift < 0) ? (ram_size >> -shift) : ram_size << shift);
		DEBUG_PRINTF("psid: %hhu - %lu\n", i, size);
		page_frame_amount[i] = size;
		error_t err = ERR_NONE;
		if(i == 0) {
			const u64 ceil_div_size_64 = (size + 63) >> 6;
			err = kmalloc(ceil_div_size_64 * sizeof(u64), (void*)&kilo_page_frame_bitmap);
			memset(kilo_page_frame_bitmap, 0, ceil_div_size_64 * sizeof(u64));

		} else {
			err = kmalloc(size * sizeof(u16), (void*)&subpage_busy_counter[i]);
			memset(subpage_busy_counter[i], 0, size * sizeof(u16));
		}
		if(err) {
			if(kilo_page_frame_bitmap) kfree(kilo_page_frame_bitmap);
			for(u8 j = 0; j < PAGE_SIZE_AMOUNT; ++j) {
				if(subpage_busy_counter[j]) kfree(subpage_busy_counter[j]);
				subpage_busy_counter[j] = nullptr;
				page_frame_amount[j] = 0;
			}
			return ERR_CRITICAL_INTERNAL_FAILURE;
		}
	}
	return ERR_NONE;
}

error_t allocate_page_frame(page_size_t page_size, ppn_t* ppnOUT) {
	if(page_size != PAGE_SIZE_4kB && subpage_busy_counter[page_size] == nullptr) return ERR_INVALID_ARGUMENT;
	u64 max_inx = 0;
	for(i8 i = PAGE_SIZE_MAX; i > page_size; --i) {
		if(subpage_busy_counter[i] == nullptr) continue;
		i32 max = -1;
		for(u64 j = max_inx; j < page_frame_amount[i]; ++j) {
			if(subpage_busy_counter[i][j] > max && subpage_busy_counter[i][j] < 511) {
				max = subpage_busy_counter[i][j];
				max_inx = j;
			}
		}
		if(max == -1) return ERR_PHISICAL_MEMORY_FULL;
		max_inx <<= 9;
	}
	if(page_size != 0) {
		for(u16 i = 0; i < 512; ++i) {
			if(subpage_busy_counter[page_size][max_inx + i] != 0) continue;
			subpage_busy_counter[page_size][max_inx + i] = 512;
			*ppnOUT = (max_inx + i) << (9 * page_size);
			break;
		}
	}
	else {
		for(u16 i = 0; i < 512; ++i) {
			if((kilo_page_frame_bitmap[(max_inx + i) >> 6] & (1 << (i & 0x3f))) != 0) continue;
			kilo_page_frame_bitmap[(max_inx + i) >> 6] |= (1 << (i & 0x3f));
			*ppnOUT = max_inx + i;
			break;
		}
	}
	for(u8 i = page_size + 1; i < 5; ++i) {
		if(subpage_busy_counter[i] == nullptr) break;
		max_inx >>= 9;
		subpage_busy_counter[i][max_inx] += 1;
		if(subpage_busy_counter[i][max_inx] != 1) break;
	}
	return ERR_NONE;
}

error_t free_page_frame(ppn_t ppn) {
	for(u8 i = 0; i < 5; ++i, ppn >>= 9) {
		if(i == 0) {
			if((kilo_page_frame_bitmap[ppn >> 6] & (1 << (ppn & 0x3f))) != 0) {
				kilo_page_frame_bitmap[ppn >> 6] &= ~(1 << (ppn & 0x3f));
				return ERR_NONE;
			}
			continue;
		}
		if(subpage_busy_counter[i] == nullptr) break;
		if(subpage_busy_counter[i][ppn] == 512) {
			subpage_busy_counter[i][ppn] = 0;
			for(u8 j = i + 1; j < 5; ++j) {
				if(subpage_busy_counter[j] == nullptr) return ERR_NONE;
				ppn >>= 9;
				--subpage_busy_counter[j][ppn];
				if(subpage_busy_counter[j][ppn] != 0) return ERR_NONE;
			}
		}
	}
	return ERR_CRITICAL_INTERNAL_FAILURE;
}

error_t set_phisical_memory_region_busy(ppn_t ppn, u64 size_in_bytes) { // TODO:
	return ERR_NONE;
}
