#include "memory_management/include/physical_memory/manager.h"

#include <logging/klog.h>

#include "allocator.h"
#include "memory_management/include/common_types.h"
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
			physical_memory_region_t new_reg = {
			    .addr = nullptr,
			    .size = phys_mem_get_frame_size_in_bytes(FRAME_SIZE_4KiB),
			};
			error_t err = phys_mem_alloc_frame(FRAME_SIZE_4KiB, &new_reg.addr);
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

u64 phys_mem_get_frame_size_in_bytes(frame_size_t fs) {
	const u64 size_4KiB = 0x1000;
	return (size_4KiB << fs);
}

// NOLINTBEGIN readability-function-cognitive-complexity
// TODO: Deal with this nolint
error_t phys_mem_init(const physical_memory_region_t* pmrs, size_t pmr_count, const memory_area_t* reserved_areas,
                      size_t reserved_areas_count) {
	static bool s_is_init = false;
	if (s_is_init)
		KLOG_DO_RETURN(ERR_REPEATED_INITIALIZATION, KLRF_TRACE_ERR);
	if (pmr_count == 0)
		KLOG_DO_RETURN(ERR_BAD_ARG, KLRF_TRACE_ERR);

	KLOG_INDENT_BLOCK_START;

	for (size_t i = 0; i < pmr_count; ++i) {
		memory_area_t pmr_area = phys_mem_reg_to_area(pmrs[i]);
		pmr_area = memory_area_shrink_to_alignment(pmr_area, 0x1000);
		memory_area_t header_area = {0};

		error_t err = pmallocator_get_header(pmr_area, reserved_areas, reserved_areas_count, &header_area);
		if (err) {
			KLOGLN_WARNING("Failed to get header region of memory region [%p - %p]", pmrs[i].addr,
			               pmrs[i].addr + pmrs[i].size);
			continue;
		}

		physical_memory_region_t header_pmr = area_to_phys_mem_reg(header_area);
		memory_region_t header_region = phys_mem_reg_to_reg(header_pmr);

		err = pmallocator_init_region(pmr_area, header_region, reserved_areas, reserved_areas_count);
		if (err) {
			KLOGLN_WARNING("Failed to init header region of memory region [%p - %p]", pmrs[i].addr,
			               pmrs[i].addr + pmrs[i].size);
			continue;
		}
		if (g_root_header_storage_node == nullptr) {
			physical_memory_region_t new_reg = {
			    .size = phys_mem_get_frame_size_in_bytes(FRAME_SIZE_4KiB),
			    .addr = nullptr,
			};
			err = pmallocator_allocate(12, header_region, &new_reg.addr);
			// NOTE: Since we failed to allocate the smallest possible frame size, we assume that this region is
			// useless.
			if (err) {
				KLOGLN_WARNING("Failed to allocate a frame of size %zu from region [%p - %p]. This region will be "
				               "ignored from now",
				               new_reg.size, pmrs[i].addr, pmrs[i].addr + pmrs[i].size);
				continue;
			}

			KLOGLN_TRACE(
			    "Memory region of size: %zu at addr: %p is used as primary node for physical region headers storage",
			    new_reg.size, new_reg.addr);
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

error_t phys_mem_alloc_frame(frame_size_t frame_size, phys_addr_t* addrOUT) {
	u32 idx = 0;
	physical_memory_region_t header_pmr;
	while (get_header_pmr(idx, &header_pmr) == ERR_NONE) {
		memory_region_t header_region = phys_mem_reg_to_reg(header_pmr);
		phys_addr_t frame_data = nullptr;
		error_t err = pmallocator_allocate(frame_size + 12, header_region, &frame_data);
		if (err) {
			++idx;
			continue;
		}
		*addrOUT = frame_data;
		KLOGLN_TRACE("Allocated a physical frame of size: %lu at %p", phys_mem_get_frame_size_in_bytes(frame_size),
		             frame_data);
		return ERR_NONE;
	}
	KLOGLN_WARNING("Allocation of physical frame of size: %lu failed", phys_mem_get_frame_size_in_bytes(frame_size));
	return ERR_OUT_OF_MEMORY;
}

error_t phys_mem_free_frame(phys_addr_t addr) {
	u32 idx = 0;
	physical_memory_region_t header_pmr;
	while (get_header_pmr(idx, &header_pmr) == ERR_NONE) {
		memory_region_t header_region = phys_mem_reg_to_reg(header_pmr);
		error_t err = pmallocator_free(addr, header_region);
		if (err) {
			++idx;
			continue;
		}
		return ERR_NONE;
	}
	KLOGLN_WARNING("Failed to free a physical frame at addr: %p", addr);
	return ERR_NOT_VALID;
}
