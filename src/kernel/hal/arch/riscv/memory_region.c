/**
 * @file memory_region.c
 * @brief RISC-V memory region enumeration implementation
 * @ingroup hal_riscv_mem
 */

#include <libboot/dt/dt.h>
#include <libcore/buffer.h>
#include <libcore/error.h>

#include "../../hal_internal.h"
#include "../../include/memory_regions.h"

static fdt_t g_hal_riscv_fdt;
static void* g_hal_riscv_fdt_addr = NULL;

static error_t hal_riscv_read_u32_cells_be(buffer_t buf, size_t offset, u32 cell_count, u64* out);

static error_t hal_riscv_init_fdt(fdt_t* fdtOUT) {
	if (fdtOUT == nullptr)
		return ERR_BAD_ARG;

	if (!ihal_is_init())
		return ERR_NOT_INITIALIZED;

	void* dtb = nullptr;
	error_t err = ihal_get_dtb(&dtb);
	if (err)
		return err;

	return dt_init(dtb, fdtOUT);
}

static error_t hal_riscv_get_fdt(const fdt_t** fdtOUT) {
	if (fdtOUT == nullptr)
		return ERR_BAD_ARG;

	void* dtb = nullptr;
	error_t err = ihal_get_dtb(&dtb);
	if (err)
		return err;

	if (g_hal_riscv_fdt_addr != dtb) {
		error_t err = hal_riscv_init_fdt(&g_hal_riscv_fdt);
		if (err)
			return err;

		g_hal_riscv_fdt_addr = dtb;
	}

	*fdtOUT = &g_hal_riscv_fdt;
	return ERR_NONE;
}

static error_t hal_riscv_node_is_memory(const fdt_t* fdt, dt_node_t node, bool* isMemoryOUT) {
	if (fdt == nullptr || isMemoryOUT == nullptr)
		return ERR_BAD_ARG;

	*isMemoryOUT = false;

	dt_prop_t device_type_prop;
	error_t err = dt_get_prop_by_name(fdt, node, "device_type", &device_type_prop);
	if (err == ERR_NONE) {
		buffer_t device_type_buf;
		err = dt_get_prop_buffer(fdt, device_type_prop, &device_type_buf);
		if (err)
			return err;

		if (buffer_equal(device_type_buf, make_buffer("memory", 6))) {
			*isMemoryOUT = true;
			return ERR_NONE;
		}
	}

	if (err != ERR_NOT_FOUND)
		return err;
	return ERR_NONE;
}

/// Finds the next memory node in the device tree.
///
/// This function has special behavior depending on the \p node parameter:
/// - When \p node is 0: Returns the first memory node (child of root)
/// - When \p node is non-zero: Returns the next sibling memory node after \p node
///
/// This dual behavior allows the function to be used both for finding the first
/// memory node and for iterating through subsequent memory nodes.
///
/// \param fdt Pointer to the device tree structure
/// \param node Current node (0 to get first memory node, or previous memory node to get next)
/// \param nodeOUT Output parameter for the found memory node
/// \return ERR_NONE on success, error code otherwise
static error_t hal_riscv_find_next_memory_node(const fdt_t* fdt, dt_node_t node, dt_node_t* nodeOUT) {
	if (fdt == nullptr || nodeOUT == nullptr)
		return ERR_BAD_ARG;

	error_t err;
	if (node != 0) {
		err = dt_get_node_sibling(fdt, node, &node);
	} else {
		err = dt_get_node_child(fdt, fdt->root_node, &node);
	}
	if (err)
		return err;

	while (true) {
		bool is_memory;
		err = hal_riscv_node_is_memory(fdt, node, &is_memory);
		if (err)
			return err;

		if (is_memory) {
			*nodeOUT = node;
			return ERR_NONE;
		}

		err = dt_get_node_sibling(fdt, node, &node);
		if (err)
			return err;
	}
}

static error_t hal_riscv_read_reg_entry(const fdt_t* fdt, dt_node_t node, u32 reg_idx, u32 address_cells,
                                        u32 size_cells, u64* addrOUT, u64* sizeOUT) {
	if (fdt == nullptr || addrOUT == nullptr || sizeOUT == nullptr)
		return ERR_BAD_ARG;

	error_t err;

	dt_prop_t reg_prop;
	err = dt_get_prop_by_name(fdt, node, "reg", &reg_prop);
	if (err)
		return err;

	buffer_t reg_buf;
	err = dt_get_prop_buffer(fdt, reg_prop, &reg_buf);
	if (err)
		return err;

	const size_t reg_entry_size = (size_t)(address_cells + size_cells) * sizeof(u32);
	const size_t entry_offset = (size_t)reg_idx * reg_entry_size;
	if (entry_offset >= reg_buf.size)
		return ERR_NOT_FOUND;
	if (reg_buf.size - entry_offset < reg_entry_size)
		return ERR_NOT_VALID;

	u64 addr;
	u64 size;
	err = hal_riscv_read_u32_cells_be(reg_buf, entry_offset, address_cells, &addr);
	if (err)
		return err;

	err = hal_riscv_read_u32_cells_be(reg_buf, entry_offset + (size_t)address_cells * sizeof(u32), size_cells, &size);
	if (err)
		return err;

	*addrOUT = addr;
	*sizeOUT = size;
	return ERR_NONE;
}

static error_t hal_riscv_read_u32_cells_be(buffer_t buf, size_t offset, u32 cell_count, u64* out) {
	if (out == nullptr || cell_count == 0 || cell_count > 2)
		return ERR_BAD_ARG;

	if (buf.size < offset)
		return ERR_NOT_VALID;
	if (buf.size - offset < (size_t)cell_count * sizeof(u32))
		return ERR_NOT_VALID;

	u64 value = 0;
	for (u32 idx = 0; idx < cell_count; ++idx) {
		u32 cell = 0;
		if (!buffer_read_u32_be(buf, offset + (size_t)idx * sizeof(u32), &cell))
			return ERR_NOT_VALID;

		value = (value << 32) | cell;
	}

	*out = value;
	return ERR_NONE;
}

typedef struct {
	u32 memreserve_idx;
	bool is_in_resmem;
	dt_node_t resmem_current_node;
	u32 resmem_address_cells;
	u32 resmem_size_cells;
	u32 resmem_reg_idx;
} riscv_hal_res_mem_iter_t;
static_assert(sizeof(riscv_hal_res_mem_iter_t) <= sizeof(hal_reserved_memory_iterator_t));

error_t hal_get_reserved_regions_iterator(hal_reserved_memory_iterator_t* iterOUT) {
	if (iterOUT == nullptr)
		return ERR_BAD_ARG;

	const fdt_t* fdt;
	error_t err = hal_riscv_get_fdt(&fdt);
	if (err)
		return err;

	dt_node_t reserved_mem_root;
	err = dt_get_node_by_path(fdt, "/reserved-memory", &reserved_mem_root);
	if (err)
		return err;

	u32 address_cells;
	u32 size_cells;
	err = dt_get_reg_cell_counts(fdt, reserved_mem_root, &address_cells, &size_cells);
	if (err)
		return err;

	dt_node_t resmem_first_node;
	err = dt_get_node_child(fdt, reserved_mem_root, &resmem_first_node);
	if (err)
		return err;

	const riscv_hal_res_mem_iter_t init = {
	    .memreserve_idx = 0,
	    .is_in_resmem = false,
	    .resmem_current_node = resmem_first_node,
	    .resmem_address_cells = address_cells,
	    .resmem_size_cells = size_cells,
	    .resmem_reg_idx = 0,
	};
	*(riscv_hal_res_mem_iter_t*)iterOUT = init;
	return ERR_NONE;
}

error_t hal_get_next_reserved_region(hal_reserved_memory_iterator_t* iter, memory_area_t* areaOUT) {
	if (iter == nullptr || areaOUT == nullptr)
		return ERR_BAD_ARG;

	const fdt_t* fdt;
	error_t err = hal_riscv_get_fdt(&fdt);
	if (err)
		return err;

	riscv_hal_res_mem_iter_t next_iter = *(riscv_hal_res_mem_iter_t*)iter;

	if (!next_iter.is_in_resmem) {
		fdt_rsv_entry entry;
		err = dt_get_rsv_mem_entry(fdt, next_iter.memreserve_idx, &entry);
		if (err == ERR_NONE) {
			memory_area_t area = {
			    .addr = (uintptr_t)entry.address,
			    .size = entry.size,
			};
			next_iter.memreserve_idx += 1;
			*(riscv_hal_res_mem_iter_t*)iter = next_iter;
			*areaOUT = area;
			return ERR_NONE;
		}

		if (err != ERR_OUT_OF_BOUNDS)
			return err;

		next_iter.is_in_resmem = true;
	}

	while (next_iter.resmem_current_node) {
		u64 addr;
		u64 size;
		err = hal_riscv_read_reg_entry(fdt, next_iter.resmem_current_node, next_iter.resmem_reg_idx,
		                               next_iter.resmem_address_cells, next_iter.resmem_size_cells, &addr, &size);
		if (err != ERR_NONE && err != ERR_NOT_FOUND) {
			return err;
		}
		if (err == ERR_NONE) {
			memory_area_t area = {
			    .addr = (uintptr_t)addr,
			    .size = size,
			};
			next_iter.resmem_reg_idx += 1;
			*(riscv_hal_res_mem_iter_t*)iter = next_iter;
			*areaOUT = area;
			return ERR_NONE;
		}

		dt_node_t sibling;
		err = dt_get_node_sibling(fdt, next_iter.resmem_current_node, &sibling);
		if (err == ERR_NONE) {
			next_iter.resmem_current_node = sibling;
			next_iter.resmem_reg_idx = 0;
			continue;
		}

		return err;
	}

	return ERR_NOT_FOUND;
}

typedef struct {
	dt_node_t node;
	u32 reg_idx;
	u32 size_cells;
	u32 address_cells;
} riscv_hal_mem_iter_t;
static_assert(sizeof(riscv_hal_mem_iter_t) <= sizeof(hal_memory_iterator_t));

error_t hal_get_memory_regions_iterator(hal_memory_iterator_t* iterOUT) {
	if (iterOUT == nullptr)
		return ERR_BAD_ARG;

	const fdt_t* fdt;
	error_t err = hal_riscv_get_fdt(&fdt);
	if (err)
		return err;

	u32 address_cells;
	u32 size_cells;
	err = dt_get_reg_cell_counts(fdt, fdt->root_node, &address_cells, &size_cells);
	if (err)
		return err;

	dt_node_t first_memory_node;
	err = hal_riscv_find_next_memory_node(fdt, 0, &first_memory_node);
	if (err)
		return err;

	const riscv_hal_mem_iter_t init = {
	    .node = first_memory_node,
	    .reg_idx = 0,
	    .size_cells = size_cells,
	    .address_cells = address_cells,
	};
	*(riscv_hal_mem_iter_t*)iterOUT = init;
	return ERR_NONE;
}

error_t hal_get_next_memory_region(hal_memory_iterator_t* iter, physical_memory_region_t* areaOUT) {
	if (iter == nullptr || areaOUT == nullptr)
		return ERR_BAD_ARG;

	riscv_hal_mem_iter_t next_iter = *(riscv_hal_mem_iter_t*)iter;

	const fdt_t* fdt;
	error_t err = hal_riscv_get_fdt(&fdt);
	if (err)
		return err;

	u32 address_cells = next_iter.address_cells;
	u32 size_cells = next_iter.size_cells;

	while (next_iter.node != 0) {
		u64 addr;
		u64 size;
		err = hal_riscv_read_reg_entry(fdt, next_iter.node, next_iter.reg_idx, address_cells, size_cells, &addr, &size);
		if (err != ERR_NONE && err != ERR_NOT_FOUND) {
			return err;
		}
		if (err == ERR_NONE) {
			const physical_memory_region_t area = {
			    .addr = (__phys void*)(uintptr_t)addr,
			    .size = size,
			};
			next_iter.reg_idx += 1;
			*(riscv_hal_mem_iter_t*)iter = next_iter;
			*areaOUT = area;
			return ERR_NONE;
		}

		dt_node_t next_memory_node;
		err = hal_riscv_find_next_memory_node(fdt, next_iter.node, &next_memory_node);
		if (err == ERR_NONE) {
			next_iter.node = next_memory_node;
			next_iter.reg_idx = 0;
			continue;
		}

		return err;
	}

	return ERR_NOT_FOUND;
}
