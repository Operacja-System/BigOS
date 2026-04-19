#include "memory_management/include/physical_memory/manager.h"

#include <logging/klog.h>
#include <stdbigos/memory_types.h>
#include <stdbigos/string.h>

#include "allocator.h"
#include "hal/memory_regions.h"
#include "stdbigos/address.h"
#include "stdbigos/error.h"
#include "stdbigos/types.h"

// ==========================================
//					Private
// ==========================================

typedef struct {
	u32 count;
	u32 max_count;
	__phys struct header_storage_node_t* next;
	physical_memory_region_t pmr_arr[];
} header_storage_node_t;

static __phys header_storage_node_t* g_root_header_storage_node = nullptr;

bool get_next_reserved_region(void* user, memory_area_t* areaOUT) {
	hal_reserved_memory_iterator_t* iter = user;
	return hal_get_next_reserved_region(iter, areaOUT) == ERR_NONE;
}

static error_t init_header_storage_node(physical_memory_region_t storage_region) {
	header_storage_node_t* effective_hsn = physical_to_effective(g_root_header_storage_node);
	effective_hsn->max_count = (storage_region.size - sizeof(header_storage_node_t)) / sizeof(physical_memory_region_t);
	effective_hsn->count = 0;
	effective_hsn->next = nullptr;
	if (effective_hsn->max_count == 0)
		return ERR_NOT_ENOUGH_MEMORY;
	return ERR_NONE;
}

[[gnu::nonnull]]
static error_t get_header_pmr(u32 idx, physical_memory_region_t* headerOUT) {
	header_storage_node_t* effective_hsn = physical_to_effective(g_root_header_storage_node);
	while (idx > (effective_hsn->count - 1)) {
		idx -= effective_hsn->count;
		if (effective_hsn->next == nullptr)
			return ERR_OUT_OF_BOUNDS;
		effective_hsn = physical_to_effective(effective_hsn->next);
	}
	*headerOUT = effective_hsn->pmr_arr[idx];
	return ERR_NONE;
}

static error_t store_header(physical_memory_region_t header) {
	header_storage_node_t* effective_hsn = physical_to_effective(g_root_header_storage_node);
	while (effective_hsn->count == effective_hsn->max_count) {
		if (effective_hsn->next != nullptr) {
			effective_hsn = physical_to_effective(effective_hsn->next);
		} else {
			physical_memory_region_t new_reg = {0};
			error_t err = phys_mem_alloc_frame(FRAME_ORDER_4KiB, &new_reg);
			if (err)
				return err;
			err = init_header_storage_node(new_reg);
			if (err)
				return err;
			KLOGLN_TRACE("New region for storage of physical region headers was added of size: %zu at addra: %p",
			             new_reg.size, new_reg.addr);
			effective_hsn->next = new_reg.addr;
		}
	}
	effective_hsn->pmr_arr[effective_hsn->count++] = header;
	return ERR_NONE;
}

// ==========================================
//					Public
// ==========================================

u64 phys_mem_get_frame_size_in_bytes(frame_order_t fs) {
	const u64 size_4KiB = 0x1000;
	return (size_4KiB << fs);
}

// NOLINTBEGIN readability-function-cognitive-complexity
// TODO: Deal with this nolint
error_t phys_mem_init() {
	static bool s_is_init = false;
	if (s_is_init)
		KLOG_DO_RETURN(ERR_REPEATED_INITIALIZATION, KLRF_TRACE_ERR);
	KLOG_INDENT_BLOCK_START;

	hal_memory_iterator_t pmr_iterator = {0};
	error_t err = hal_get_memory_regions_iterator(&pmr_iterator);
	if (err == ERR_NOT_INITIALIZED) {
		KLOGLN_ERROR("HAL failed with error: %d during pmm initialization", err);
		return ERR_NOT_VALID;
	}

	physical_memory_region_t pmr = {0};
	while (hal_get_next_memory_region(&pmr_iterator, &pmr) == ERR_NONE) {
		pmr = physical_memory_region_shrink_to_alignment(pmr, 0x1000);

		memory_area_t header_area = {0};
		memory_area_t pmr_area = physical_memory_region_to_area(pmr);
		hal_reserved_memory_iterator_t iter = {0};
		err = hal_get_reserved_regions_iterator(&iter);
		if (err) {
			KLOGLN_ERROR("Failed to reset reserved memory regions iterator");
			return ERR_NOT_VALID;
		}
		err = pmallocator_get_header(pmr_area, get_next_reserved_region, &iter, &header_area);
		if (err) {
			KLOGLN_WARNING("Failed to get header region of memory region [%p - %p]", pmr.addr, pmr.addr + pmr.size);
			continue;
		}

		physical_memory_region_t header_pmr = {
		    .addr = (__phys void*)header_area.addr,
		    .size = header_area.size,
		};
		memory_region_t header_eff_reg = physical_memory_region_to_effective(header_pmr);

		err = hal_get_reserved_regions_iterator(&iter);
		if (err) {
			KLOGLN_ERROR("Failed to reset reserved memory regions iterator");
			return ERR_NOT_VALID;
		}
		err = pmallocator_init_region(pmr_area, header_eff_reg, get_next_reserved_region, &iter);
		if (err) {
			KLOGLN_WARNING("Failed to init header region of memory region [%p - %p]", pmr.addr, pmr.addr + pmr.size);
			continue;
		}
		if (g_root_header_storage_node == nullptr) {
			memory_area_t new_area = {0};
			err = pmallocator_allocate(FRAME_ORDER_4KiB, header_eff_reg, &new_area);
			// NOTE: Since we failed to allocate the smallest possible frame size, we assume that this region is
			// useless.
			if (err) {
				KLOGLN_WARNING("Failed to allocate a frame of size %zu from region [%p - %p]. This region will be "
				               "ignored from now",
				               new_area.size, pmr.addr, pmr.addr + pmr.size);
				continue;
			}

			KLOGLN_TRACE(
			    "Memory region of size: %zu at addr: %p is used as primary node for physical region headers storage",
			    new_area.size, (void*)new_area.addr);
			physical_memory_region_t new_reg = {
			    .addr = (__phys void*)new_area.addr,
			    .size = new_area.size,
			};
			err = init_header_storage_node(new_reg);
			if (err) {
				// No need to free this region since this error is, and should be, fatal
				KLOG_ERROR("4KiB was not enough to store any header regions. A single header region should be nowhere "
				           "near this size.");
				KLOG_DO_RETURN(ERR_NOT_ENOUGH_MEMORY, KLRF_TRACE_ERR | KLRF_END_BLOCK);
			}

			g_root_header_storage_node = new_reg.addr;
		}

		err = store_header(header_pmr);
		if (err)
			KLOG_DO_RETURN(ERR_OUT_OF_BOUNDS, KLRF_TRACE_ERR | KLRF_END_BLOCK);
	}
	s_is_init = true;
	KLOG_DO_RETURN(ERR_NONE, KLRF_END_BLOCK);
}
// NOLINTEND readability-function-cognitive-complexity

error_t phys_mem_alloc_frame(frame_order_t frame_size, physical_memory_region_t* regOUT) {
	u32 idx = 0;
	physical_memory_region_t header_pmr;
	while (get_header_pmr(idx, &header_pmr) == ERR_NONE) {
		memory_region_t header_region = physical_memory_region_to_effective(header_pmr);
		memory_area_t area_out;
		error_t err = pmallocator_allocate(frame_size, header_region, &area_out);
		if (err) {
			++idx;
			continue;
		}
		physical_memory_region_t pmr_out = {
		    .addr = (__phys void*)area_out.addr,
		    .size = area_out.size,
		};
		*regOUT = pmr_out;
		KLOGLN_TRACE("Allocated a physical frame of size: %lu at %p", phys_mem_get_frame_size_in_bytes(frame_size),
		             pmr_out.addr);
		return ERR_NONE;
	}
	KLOGLN_WARNING("Allocation of physical frame of size: %lu failed", phys_mem_get_frame_size_in_bytes(frame_size));
	return ERR_OUT_OF_MEMORY;
}

error_t phys_mem_free_frame(physical_memory_region_t reg) {
	u32 idx = 0;
	physical_memory_region_t header_pmr;
	while (get_header_pmr(idx, &header_pmr) == ERR_NONE) {
		memory_region_t header_region = physical_memory_region_to_effective(header_pmr);
		memory_area_t area = physical_memory_region_to_area(reg);
		error_t err = pmallocator_free(area, header_region);
		if (err) {
			++idx;
			continue;
		}
		return ERR_NONE;
	}
	KLOGLN_WARNING("Failed to free a physical frame at addr: %p, of size: %zu", reg.addr, reg.size);
	return ERR_NOT_VALID;
}
