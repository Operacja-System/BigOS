#include "page_table.h"

#include <debug/debug_stdio.h>
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>

#include "kernel_config.h"
#include "klog.h"
#include "memory_managment/mm_types.h"
#include "memory_managment/physical_memory_manager.h"
#include "ram_map.h"

// ========================================
//				  Private
// ========================================

typedef u64 raw_pte_t;

[[nodiscard]]
static page_table_entry_t read_raw_pte(raw_pte_t raw_pte) {
	const page_table_entry_t pte = {
	    .flags = raw_pte & 0xff,
	    .os_flags = (raw_pte >> 8) & 0b11,
	    .ppn = (raw_pte >> 10) & 0xfffffffffff,
	    .pbmt = (raw_pte >> 60) & 0b11,
	    .N = (raw_pte >> 62) & 0b1,
	};
	return pte;
}

[[nodiscard]]
static raw_pte_t write_raw_pte(page_table_entry_t pte) {
	raw_pte_t raw_pte = 0;
	raw_pte |= (u64)pte.flags & 0xff;
	raw_pte |= (u64)(pte.os_flags & 0b11) << 8;
	raw_pte |= (u64)(pte.ppn & 0xfffffffffff) << 10;
	raw_pte |= (u64)(pte.pbmt & 0b11) << 60;
	raw_pte |= (u64)(pte.N & 0b1) << 62;
	return raw_pte;
}

[[nodiscard]]
static inline bool is_pte_leaf(page_table_entry_t pte) {
	return (pte.flags & (PTEF_READ | PTEF_WRITE | PTEF_EXECUTE)) != 0;
}

[[nodiscard]]
static bool is_pointer_pt_empty(page_table_entry_t pte) {
	raw_pte_t* raw_page_table = physical_to_effective(ppn_to_phys_addr(pte.ppn, 0));
	for (u16 i = 0; i < 512; ++i) {
		page_table_entry_t inner_pte = read_raw_pte(raw_page_table[i]);
		if (inner_pte.flags & PTEF_VALID)
			return false;
	}
	return true;
}

[[nodiscard]]
static error_t delete_page_table(page_table_entry_t root_pte) {
	typedef struct {
		page_table_entry_t pte;
		u16 idx;
	} stack_entry_t;
	u8 pt_height = 0;
	if (!buffer_read_u8(kernel_config_get(KERCFG_PT_HEIGHT), 0, &pt_height))
		return ERR_INTERNAL_FAILURE;
	const size_t stack_size = pt_height + 2;
	stack_entry_t stack[stack_size];
	memset(stack, 0, sizeof(stack));
	u8 stack_end_idx = 0;

	stack[stack_end_idx++] = (stack_entry_t){
	    .pte = root_pte,
	    .idx = 0,
	};

	while (stack_end_idx != 0) {
		if (stack_end_idx >= stack_size - 1)
			return ERR_BAD_ARG; // This may leave dangling pages, but if this happened the page table is broken anyways
		stack_entry_t* stack_back = &stack[stack_end_idx - 1];
		if (!(stack_back->pte.flags & PTEF_VALID)) {
			--stack_end_idx;
			continue;
		}
		if (stack_back->idx >= 512) {
			const error_t err = phys_mem_free_frame(stack_back->pte.ppn);
			if (err)
				return ERR_INTERNAL_FAILURE;
			--stack_end_idx;
			continue;
		}
		if (is_pte_leaf(stack_back->pte)) {
			if (!(stack_back->pte.os_flags & PTEOSF_MAPPED)) {
				const error_t err = phys_mem_free_frame(stack_back->pte.ppn);
				if (err)
					return ERR_INTERNAL_FAILURE;
			}
			--stack_end_idx;
		} else {
			raw_pte_t* page_table = physical_to_effective(ppn_to_phys_addr(stack_back->pte.ppn, 0));
			page_table_entry_t next_pte = read_raw_pte(page_table[stack_back->idx++]);
			stack[stack_end_idx++] = (stack_entry_t){
			    .pte = next_pte,
			    .idx = 0,
			};
		}
	}
	return ERR_NONE;
}

static void print_pte(page_table_entry_t pte, u8 depth, u8 pt_height, const u16 vaddr_slice[5]) {
	dputgap(depth);
	if (!is_pte_leaf(pte)) {
		KLOGLN_NOTE("Pointer page - ppn: #%lx", pte.ppn);
		return;
	}
	u64 vaddr_begin = ((u64)vaddr_slice[4] & 0x1ff) << (9 * 4) | ((u64)vaddr_slice[3] & 0x1ff) << (9 * 3) |
	                  ((u64)vaddr_slice[2] & 0x1ff) << (9 * 2) | ((u64)vaddr_slice[1] & 0x1ff) << (9 * 1) |
	                  ((u64)vaddr_slice[0] & 0x1ff) << (9 * 0);
	vaddr_begin <<= 12;
	u64 vaddr_end = vaddr_begin + page_size_get_in_bytes(pt_height - depth);
	char flags[9];
	char os_flags[3];
	flags[8] = '\0';
	flags[7] = (pte.flags & PTEF_VALID) ? 'V' : '-';
	flags[6] = (pte.flags & PTEF_READ) ? 'R' : '-';
	flags[5] = (pte.flags & PTEF_WRITE) ? 'W' : '-';
	flags[4] = (pte.flags & PTEF_EXECUTE) ? 'X' : '-';
	flags[3] = (pte.flags & PTEF_USER) ? 'U' : '-';
	flags[2] = (pte.flags & PTEF_GLOBAL) ? 'G' : '-';
	flags[1] = (pte.flags & PTEF_ACCESSED) ? 'A' : '-';
	flags[0] = (pte.flags & PTEF_DIRTY) ? 'D' : '-';
	os_flags[2] = '\0';
	os_flags[1] = (pte.os_flags & PTEOSF_TO_BE_DEFINED) ? '*' : '-';
	os_flags[0] = (pte.os_flags & PTEOSF_MAPPED) ? 'M' : '-';
	KLOGLN_NOTE("Leaf page (%016lx - %016lx) - os_flags[%s] - flags[%s] - ppn: #%lx", vaddr_begin, vaddr_end, os_flags,
	            flags, pte.ppn);
}

// ========================================
//					Public
// ========================================

error_t page_table_create(page_table_entry_t* page_tableOUT) {
	*page_tableOUT = (page_table_entry_t){0};
	return ERR_NONE;
}

error_t page_table_destroy(page_table_entry_t* page_table) {
	if (!(page_table->flags & PTEF_VALID))
		return ERR_NOT_VALID;
	const error_t err = delete_page_table(*page_table);
	if (err)
		return err;
	*page_table = (page_table_entry_t){0};
	return ERR_NONE;
}

error_t page_table_add_entry(const page_table_entry_t* page_table, page_size_t ps, vpn_t vpn,
                             page_table_entry_t entry) {
	u16 vpn_slice[5] = {
	    (vpn >> 9 * 0) & 0x1ff, (vpn >> 9 * 1) & 0x1ff, (vpn >> 9 * 2) & 0x1ff,
	    (vpn >> 9 * 3) & 0x1ff, (vpn >> 9 * 4) & 0x1ff,
	};
	const char* log_prefix[] = {"kilo", "mega", "giga", "tera", "peta"};
	KLOGLN_TRACE("Adding a %sframe #%lx to page table with root ppn: #%lx.", log_prefix[ps], entry.ppn,
	             page_table->ppn);

	KLOG_INDENT_BLOCK_START;
	raw_pte_t* current_page = physical_to_effective(ppn_to_phys_addr(page_table->ppn, 0));

	u8 pt_height = 0;
	if (!buffer_read_u8(kernel_config_get(KERCFG_PT_HEIGHT), 0, &pt_height)) {
		KLOGLN_TRACE("Failed to read kernel config.");
		KLOG_END_BLOCK_AND_RETURN(ERR_INTERNAL_FAILURE);
	}

	for (i32 lvl = pt_height - 1; lvl > ps; --lvl) {
		raw_pte_t* current_raw_pte = &(current_page[vpn_slice[lvl]]);
		page_table_entry_t current_pte = read_raw_pte(*current_raw_pte);
		if ((current_pte.flags & PTEF_VALID) == 0) {
			ppn_t new_ppn = 0;
			error_t err = phys_mem_alloc_frame(PAGE_SIZE_4kB, &new_ppn);
			if (err) {
				KLOGLN_TRACE("Failed to allocate a frame.");
				KLOG_END_BLOCK_AND_RETURN(err);
			}
			current_pte.ppn = new_ppn;
			memset(physical_to_effective(ppn_to_phys_addr(current_pte.ppn, 0)), 0,
			       page_size_get_in_bytes(PAGE_SIZE_4kB));
			current_pte.flags = PTEF_VALID;
			current_pte.flags |= entry.flags & (PTEF_GLOBAL | PTEF_USER);
			KLOGLN_TRACE("Adding a pointer frame of ppn: #%lx at index: %u...", new_ppn, vpn_slice[lvl]);
			*current_raw_pte = write_raw_pte(current_pte);
		}
		current_page = physical_to_effective(ppn_to_phys_addr(current_pte.ppn, 0));
	}
	page_table_entry_t target_pte = read_raw_pte(current_page[vpn_slice[ps]]);
	if (target_pte.flags & PTEF_VALID)
		KLOG_END_BLOCK_AND_RETURN(ERR_BAD_ARG);
	KLOGLN_TRACE("Adding a target frame of ppn: #%lx at index: %u...", entry.ppn, vpn_slice[ps]);
	current_page[vpn_slice[ps]] = write_raw_pte(entry);
	KLOG_END_BLOCK_AND_RETURN(ERR_NONE);
}

// This leaves empty pointer pages however I don't think this will create a noticable memory overhead, and will
// marginally speed-up mapping a region
error_t page_table_remove_region(page_table_entry_t* root_pte, virt_mem_region_t region) {
	void* current_address = region.addr;
	while (current_address < region.addr + region.size) {
		vpn_t vpn = virt_addr_to_vpn(current_address);
		u16 vpn_slice[5] = {
		    (vpn >> 9 * 0) & 0x1ff, (vpn >> 9 * 1) & 0x1ff, (vpn >> 9 * 2) & 0x1ff,
		    (vpn >> 9 * 3) & 0x1ff, (vpn >> 9 * 4) & 0x1ff,
		};
		u8 pt_height = 0;
		if (!buffer_read_u8(kernel_config_get(KERCFG_PT_HEIGHT), 0, &pt_height)) {
			KLOGLN_TRACE("Failed to read buffer.");
			return ERR_INTERNAL_FAILURE;
		}

		if ((root_pte->flags & PTEF_VALID) == 0)
			return ERR_NOT_VALID;
		ppn_t current_ppn = root_pte->ppn;

		for (i32 lvl = pt_height - 1; lvl >= 0; --lvl) {
			raw_pte_t* raw_page_table = physical_to_effective(ppn_to_phys_addr(current_ppn, 0));
			raw_pte_t* raw_page_table_entry = &raw_page_table[vpn_slice[lvl]];
			page_table_entry_t pte = read_raw_pte(*raw_page_table_entry);

			if ((pte.flags & PTEF_VALID) == 0)
				return ERR_NOT_VALID;

			if (is_pte_leaf(pte)) {
				*raw_page_table_entry = 0;
				if (!(pte.os_flags & PTEOSF_MAPPED)) {
					error_t err = phys_mem_free_frame(pte.ppn);
					if (err)
						return err;
				}
				current_address += page_size_get_in_bytes(lvl);
				break;
			}
			current_ppn = pte.ppn;
		}
	}
	return ERR_NONE;
}

error_t page_table_walk(page_table_entry_t* page_table, void* vaddr, phys_addr_t* paddrOUT) {
	vpn_t vpn = virt_addr_to_vpn(vaddr);
	u16 vpn_slice[5] = {
	    (vpn >> 9 * 0) & 0x1ff, (vpn >> 9 * 1) & 0x1ff, (vpn >> 9 * 2) & 0x1ff,
	    (vpn >> 9 * 3) & 0x1ff, (vpn >> 9 * 4) & 0x1ff,
	};
	u8 pt_height = 0;
	if (!buffer_read_u8(kernel_config_get(KERCFG_PT_HEIGHT), 0, &pt_height)) {
		KLOGLN_TRACE("Failed to read buffer.");
		return ERR_INTERNAL_FAILURE;
	}

	if ((page_table->flags & PTEF_VALID) == 0)
		return ERR_NOT_VALID;
	ppn_t current_ppn = page_table->ppn;

	for (i32 lvl = pt_height - 1; lvl >= 0; --lvl) {
		const raw_pte_t* raw_page_table = physical_to_effective(ppn_to_phys_addr(current_ppn, 0));
		const raw_pte_t raw_page_table_entry = raw_page_table[vpn_slice[lvl]];
		page_table_entry_t pte = read_raw_pte(raw_page_table_entry);

		if ((pte.flags & PTEF_VALID) == 0)
			return ERR_NOT_FOUND;

		current_ppn = pte.ppn;
		if (is_pte_leaf(pte)) {
			u64 ret_addr = ppn_to_phys_addr(current_ppn, 0);
			const u64 addr_offset_mask = (1ull << (12 + 9 * lvl)) - 1;
			const u64 addr_offset = (u64)vaddr & addr_offset_mask;
			ret_addr &= ~addr_offset_mask;
			ret_addr |= addr_offset;
			*paddrOUT = ret_addr;
			return ERR_NONE;
		}
	}
	return ERR_NOT_VALID; // Should never happen (if this happens page table is invalid)
}

// This returns a bool instead of an error so that one can check if the reason nothing is printing is that this function
// broke, but since this is mostly for debugging I don't want this code to be responsible for checking if the page table
// is correct.
bool page_table_print(page_table_entry_t root_pte) {
	typedef struct {
		page_table_entry_t pte;
		u16 idx;
		bool printed;
	} stack_entry_t;
	u8 pt_height = 0;
	if (!buffer_read_u8(kernel_config_get(KERCFG_PT_HEIGHT), 0, &pt_height))
		return false;
	const size_t stack_size = pt_height + 2;
	stack_entry_t stack[stack_size];
	memset(stack, 0, sizeof(stack));
	u8 stack_end_idx = 0;

	stack[stack_end_idx++] = (stack_entry_t){
	    .pte = root_pte,
	    .idx = 0,
	    .printed = false,
	};

	while (stack_end_idx != 0) {
		if (stack_end_idx >= stack_size - 1)
			return false;
		stack_entry_t* stack_back = &stack[stack_end_idx - 1];
		if (!(stack_back->pte.flags & PTEF_VALID)) {
			--stack_end_idx;
			continue;
		}
		if (stack_back->idx >= 512) {
			--stack_end_idx;
			continue;
		}
		if (!stack_back->printed) {
			u16 vaddr_slice[5];
			memset(vaddr_slice, 0, sizeof(vaddr_slice));
			for (u8 i = 0; i < stack_end_idx - 1; ++i) {
				vaddr_slice[pt_height - 1 - i] = stack[i].idx - 1;
			}
			print_pte(stack_back->pte, stack_end_idx - 1, pt_height, vaddr_slice);
			stack_back->printed = true;
		}
		if (is_pte_leaf(stack_back->pte)) {
			--stack_end_idx;
		} else {
			raw_pte_t* page_table = physical_to_effective(ppn_to_phys_addr(stack_back->pte.ppn, 0));
			page_table_entry_t next_pte = read_raw_pte(page_table[stack_back->idx++]);
			stack[stack_end_idx++] = (stack_entry_t){
			    .pte = next_pte,
			    .idx = 0,
			    .printed = false,
			};
		}
	}
	return true;
}
