#include "memory_managment/address_space_manager.h"

#include <stdbigos/buffer.h>
#include <stdbigos/string.h>

#include "klog.h"
#include "memory_managment/mm_types.h"
#include "memory_managment/page_table.h"
#include "memory_managment/virtual_memory_managment.h"
#include "ram_map.h"

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
		return ERR_NOT_INITIALIZED;
#endif
	ashOUT->valid = false;
	ashOUT->user = user;
	ashOUT->global = global;
	ashOUT->asid = 0;
	ashOUT->generation = 1; // Not 0 to detect overflow
	ashOUT->root_pte = 0;
	return ERR_NONE;
}

error_t address_space_destroy(as_handle_t* ash) {
#ifdef __DEBUG__
	if (!s_is_init)
		return ERR_NOT_INITIALIZED;
#endif
	page_table_entry_t pte = {0};
	pte.ppn = ash->root_pte;
	pte.N = 0;
	pte.pbmt = 0;
	pte.flags = PTEF_VALID;
	const error_t err = page_table_destroy(&pte);
	if (err)
		return err;
	*ash = (as_handle_t){.asid = 0, .root_pte = 0, .valid = false, .global = false, .user = false};
	return ERR_NONE;
}

error_t address_space_add_region(as_handle_t* ash, virt_mem_region_t region) {
#ifdef __DEBUG__
	if (!s_is_init)
		return ERR_NOT_INITIALIZED;
#endif
	KLOGLN_TRACE("Adding a region to address space");
	KLOG_INDENT_BLOCK_START;
	// This is not created in address_space_create so that empty address spaces wont have memory overhead
	if (!ash->valid) {
		ppn_t ppn = 0;
		error_t err = phys_mem_alloc_frame(PAGE_SIZE_4kB, &ppn);
		if (err) {
			KLOGLN_TRACE("Failed to allocate a frame for the root page");
			KLOG_END_BLOCK_AND_RETURN(err);
		}
		void* page_table_page = physical_to_effective(ppn_to_phys_addr(ppn, 0));
		memset(page_table_page, 0, page_size_get_in_bytes(PAGE_SIZE_4kB));
		ash->root_pte = ppn;
		ash->valid = true;
	}

	u8 flags = PTEF_VALID;
	flags |= PTEF_READ * region.read;
	flags |= PTEF_WRITE * region.write;
	flags |= PTEF_EXECUTE * region.execute;
	flags |= PTEF_USER * region.user;
	flags |= PTEF_GLOBAL * region.global;

	const u64 delta_size = page_size_get_in_bytes(region.ps);
	const u8 root_flags = PTEF_VALID | ((int)(ash->global) ? PTEF_GLOBAL : 0) | ((int)(ash->user) ? PTEF_USER : 0);
	const page_table_entry_t root_pte = {.flags = root_flags, .os_flags = 0, .ppn = ash->root_pte, .pbmt = 0, .N = 0};

	void* curr_addr = region.addr;
	phys_addr_t curr_map_addr = region.map_region.addr;

	while (curr_addr < region.addr + region.size) {
		ppn_t ppn = 0;
		if (region.mapped) {
			ppn = phys_addr_to_ppn(curr_map_addr);
			curr_map_addr += delta_size;
		} else {
			error_t err = phys_mem_alloc_frame(region.ps, &ppn);
			if (err)
				KLOG_END_BLOCK_AND_RETURN(err);
		}
		page_table_entry_t new_entry = {
		    .flags = flags,
		    .os_flags = 0,
		    .ppn = ppn,
		    .pbmt = 0,
		    .N = 0,
		};
		error_t err = page_table_add_entry(&root_pte, region.ps, virt_addr_to_vpn(curr_addr), new_entry);
		if (err)
			KLOG_END_BLOCK_AND_RETURN(err);
		curr_addr += delta_size;
	}
	KLOG_END_BLOCK_AND_RETURN(ERR_NONE);
}

error_t address_space_set_stack_data(as_handle_t* ash, void* stack_start, size_t stack_size) {
#ifdef __DEBUG__
	if (!s_is_init)
		return ERR_NOT_INITIALIZED;
#endif
	vpn_t stack_end_vpn = ((vpn_t)stack_start - stack_size) >> 12;
	ash->stack_top_page = stack_end_vpn;
	return ERR_NONE;
}

error_t address_space_set_heap_data(as_handle_t* ash, void* heap_start, size_t heap_size) {
#ifdef __DEBUG__
	if (!s_is_init)
		return ERR_NOT_INITIALIZED;
#endif
	vpn_t heap_end_vpn = ((vpn_t)heap_start + heap_size) >> 12;
	ash->heap_top_page = heap_end_vpn;
	return ERR_NONE;
}

error_t address_space_vaddr_to_paddr(as_handle_t* ash, void* vaddr, phys_addr_t* paddrOUT) {
	if (!ash->valid)
		return ERR_NOT_VALID;
	u8 flags = 0;
	flags |= PTEF_VALID * ash->valid;
	flags |= PTEF_GLOBAL * ash->global;
	flags |= PTEF_USER * ash->user;
	page_table_entry_t pte = {
	    .ppn = ash->root_pte,
	    .flags = flags,
	    .N = 0,
	    .pbmt = 0,
	};
	return page_table_walk(&pte, vaddr, paddrOUT);
}

void address_space_print_page_table(as_handle_t* ash) {
	u8 flags = 0;
	if (ash->valid)
		flags |= PTEF_VALID;
	if (ash->global)
		flags |= PTEF_GLOBAL;
	if (ash->user)
		flags |= PTEF_USER;
	page_table_entry_t pte = {.flags = flags, .os_flags = 0, .N = 0, .pbmt = 0, .ppn = ash->root_pte};
	page_table_print(pte);
}
