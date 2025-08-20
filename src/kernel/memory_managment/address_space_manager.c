#include "memory_managment/address_space_manager.h"

#include <stdbigos/buffer.h>
#include <stdbigos/string.h>

#include "debug/debug_stdio.h"
#include "kernel_config.h"
#include "memory_managment/page_table.h"
#include "memory_managment/virtual_memory_managment.h"
#include "ram_map.h"
#include "klog.h"

static u16 s_max_asid = 0;
static u16 s_next_asid = 0;
static u64 s_generation = 0;
#ifdef __DEBUG__
static bool s_is_init = false;
#endif

/// Private

// Necessary after every change in the address space (manages TBL)
static void address_space_update_asid(as_handle_t* ash) {
	ash->asid = s_next_asid++;
	if (ash->asid > s_max_asid || ash->asid == 0) {
		s_next_asid = 0;
		ash->asid = s_next_asid++;
		ash->generation = ++s_generation;
		if (s_generation == 0) {
			// TODO: Technically this should emit a warning, because if the OS runs for more than
			// 58454204.608 years, generation overflow will allow ASIDs that have not beed used since boot to alias.
		}
		virt_mem_flush_TLB();
		KLOGLN_TRACE("ASID generation advanced to: %lu; TLB flushed.", s_generation);
	}
}

// Necessary after every time the address space is set active (will create more problems if concurency is allowed)
static inline void address_space_verify_asid(as_handle_t* ash) {
	if (ash->generation != s_generation) {
		address_space_update_asid(ash);
	}
}

/// Public

error_t address_space_managment_init(u16 max_asid) {
	s_max_asid = max_asid;
#ifdef __DEBUG__
	s_is_init = true;
#endif
	return ERR_NONE;
}

error_t address_space_create(as_handle_t* ashOUT, bool user, bool global) {
#ifdef __DEBUG__
	if (!s_is_init)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_INITIALIZED);
#endif
	ashOUT->valid = false;
	ashOUT->user = user;
	ashOUT->global = global;
	ashOUT->asid = 0;
	ashOUT->root_pte = 0;
	return ERR_NONE;
}

error_t address_space_destroy(as_handle_t* ash) {
#ifdef __DEBUG__
	if (!s_is_init)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_INITIALIZED);
#endif
	page_table_entry_t pte = {0};
	pte.ppn = ash->root_pte;
	pte.N = 0;
	pte.pbmt = 0;
	pte.flags = PTEF_VALID;
	const error_t err = page_table_destroy(&pte);
	if (err)
		KLOG_RETURN_ERR_TRACE(err);
	*ash = (as_handle_t){.asid = 0, .root_pte = 0, .valid = false, .global = false, .user = false};
	return ERR_NONE;
}

error_t address_sapce_add_region(as_handle_t* ash, virt_mem_region_t region) {
#ifdef __DEBUG__
	if (!s_is_init)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_INITIALIZED);
#endif
	KLOGLN_TRACE("Adding a region to address space");
	KLOG_INDENT_BLOCK_START;
	if (!ash->valid) {
		ppn_t ppn = 0;
		error_t err = phys_mem_alloc_frame(PAGE_SIZE_4kB, &ppn);
		if (err) {
			KLOGLN_TRACE("Failed to allocate a frame for the root page");
			KLOG_END_BLOCK_AND_RETURN_ERR_TRACE(err);
		}
		void* page_table_page = physical_to_effective(ppn << 12);
		memset(page_table_page, 0, 0x1000);
		ash->root_pte = ppn;
		ash->valid = true;
	}
	void* curr_addr = region.addr;
	size_t size_left = region.size;
	phys_addr_t curr_map_addr = region.map_region.addr;

	u8 pt_height = 0;

	buffer_t pt_height_buffer = kernel_config_get(KERCFG_PT_HEIGHT);
	if (pt_height_buffer.error)
		KLOG_END_BLOCK_AND_RETURN_ERR_TRACE(ERR_INTERNAL_FAILURE);
	const error_t err = buffer_read_u8(pt_height_buffer, 0, &pt_height);
	if (err)
		KLOG_END_BLOCK_AND_RETURN_ERR_TRACE(ERR_INTERNAL_FAILURE);

	u8 flags = PTEF_VALID;
	if (region.read)
		flags |= PTEF_READ;
	if (region.write)
		flags |= PTEF_WRITE;
	if (region.execute)
		flags |= PTEF_EXECUTE;
	if (region.user)
		flags |= PTEF_USER;
	if (region.global)
		flags |= PTEF_GLOBAL;

	const size_t delta_size = 0x1000 << (9 * region.ps);
	while (size_left > 0) {
		ppn_t ppn = 0;
		if (region.mapped) {
			ppn = curr_map_addr >> 12;
			curr_map_addr += delta_size;
		} else {
			error_t err = phys_mem_alloc_frame(region.ps, &ppn);
			if (err)
				KLOG_END_BLOCK_AND_RETURN_ERR_TRACE(err);
		}
		const u8 root_flags = PTEF_VALID | ((int)(ash->global) ? PTEF_GLOBAL : 0) | ((int)(ash->user) ? PTEF_USER : 0);
		page_table_entry_t root_pte = {.flags = root_flags, .os_flags = 0, .ppn = ash->root_pte, .pbmt = 0, .N = 0};
		page_table_entry_t new_entry = {
		    .flags = flags,
		    .os_flags = 0,
		    .ppn = ppn,
		    .pbmt = 0,
		    .N = 0,
		};
		error_t err = page_table_add_entry(&root_pte, region.ps, (u64)curr_addr >> 12, new_entry);
		if (err)
			KLOG_END_BLOCK_AND_RETURN_ERR_TRACE(err);
		size_left -= delta_size;
		curr_addr += delta_size;
	}
	KLOG_END_BLOCK_AND_RETURN(ERR_NONE);
}

error_t address_space_set_stack_data(as_handle_t* ash, void* stack_start, size_t stack_size) {
#ifdef __DEBUG__
	if (!s_is_init)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_INITIALIZED);
#endif
	vpn_t stack_end_vpn = ((vpn_t)stack_start - stack_size) >> 12;
	ash->stack_top_page = stack_end_vpn;
	return ERR_NONE;
}

error_t address_space_set_heap_data(as_handle_t* ash, void* heap_start, size_t heap_size) {
#ifdef __DEBUG__
	if (!s_is_init)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_INITIALIZED);
#endif
	vpn_t heap_end_vpn = ((vpn_t)heap_start + heap_size) >> 12;
	ash->heap_top_page = heap_end_vpn;
	return ERR_NONE;
}

