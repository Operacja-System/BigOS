#include "page_table.h"

#include <debug/debug_stdio.h>
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>

#include "kernel_config.h"
#include "klog.h"
#include "memory_managment/physical_memory_manager.h"
#include "ram_map.h"

// ========================================
//				  Private
// ========================================

static constexpr u32 pt_entries_amount = 512;

static page_table_entry_t read_riscv_pte(u64 riscv_pte) {
	u8 flags = riscv_pte & 0xff;
	u8 rsw = (riscv_pte >> 8) & 0b11;
	ppn_t ppn = (riscv_pte >> 10) & 0xfffffffffff;
	u8 pbmt = (riscv_pte >> 60) & 0b11;
	u8 N = (riscv_pte >> 62) & 0b1;
	return (page_table_entry_t){.flags = flags, .N = N, .ppn = ppn, .pbmt = pbmt, .os_flags = rsw};
}

static u64 write_riscv_pte(page_table_entry_t pte) {
	u64 riscv_pte = 0;
	riscv_pte |= (u64)pte.flags & 0xff;
	riscv_pte |= (u64)(pte.os_flags & 0b11) << 8;
	riscv_pte |= (u64)(pte.ppn & 0xfffffffffff) << 10;
	riscv_pte |= (u64)(pte.pbmt & 0b11) << 61;
	riscv_pte |= (u64)(pte.N & 0b1) << 63;
	return riscv_pte;
}

static inline bool is_pte_leaf(page_table_entry_t pte) {
	return (pte.flags & PTEF_READ || pte.flags & PTEF_WRITE || pte.flags & PTEF_EXECUTE) != 0;
}

static void delete_pte(page_table_entry_t pte) {
	if (is_pte_leaf(pte)) {
		const error_t err = phys_mem_free_frame(pte.ppn);
		if (err) { /*TODO: Panic*/
		}
		return;
	}
	u64(*pt)[pt_entries_amount] = physical_to_effective(pte.ppn << 12);
	for (u32 i = 0; i < pt_entries_amount; ++i) {
		page_table_entry_t new_pte = read_riscv_pte((*pt)[i]);
		if (new_pte.flags & PTEF_VALID)
			delete_pte(new_pte);
	}
	const error_t err = phys_mem_free_frame(pte.ppn);
	if (err) { /*TODO: Panic*/
	}
}

void print_pte(page_table_entry_t pte, u8 lvl, u8 h, u64 virt_addr) {
	if (!(pte.flags & PTEF_VALID)) {
		return;
	}
	if (is_pte_leaf(pte)) {
		for (u8 i = 0; i < lvl; ++i) DEBUG_PUTC('\t');
		char os_flags[2 + 1] = "--";
		char flags[8 + 1] = "--------";
		if (pte.flags & PTEF_VALID)
			flags[7] = 'V';
		if (pte.flags & PTEF_READ)
			flags[6] = 'R';
		if (pte.flags & PTEF_WRITE)
			flags[5] = 'W';
		if (pte.flags & PTEF_EXECUTE)
			flags[4] = 'X';
		if (pte.flags & PTEF_USER)
			flags[3] = 'U';
		if (pte.flags & PTEF_GLOBAL)
			flags[2] = 'G';
		if (pte.flags & PTEF_ACCESSED)
			flags[1] = 'A';
		if (pte.flags & PTEF_DIRTY)
			flags[0] = 'D';
		const char* ps_prefix[5] = {"peta", "tera", "giga", "mega", "kilo"};
		u64 va_print = virt_addr << ((9 * (h - lvl)) + 12);
		DEBUG_PRINTF("addr: %016lx - ps: %spage - os flags: %s - flags: %s\n", va_print, ps_prefix[lvl], os_flags,
		             flags);
		return;
	}
	ppn_t ppn = pte.ppn;
	u64* current_page = physical_to_effective(ppn << 12);
	for (u32 i = 0; i < pt_entries_amount; ++i) {
		u64 riscv_pte = current_page[i];
		page_table_entry_t new_pte = read_riscv_pte(riscv_pte);
		print_pte(new_pte, lvl + 1, h, (virt_addr << 9) | i);
	}
}

// ========================================
//					Public
// ========================================

error_t page_table_create(page_table_entry_t* page_tableOUT) {
	*page_tableOUT = (page_table_entry_t){0};
	return ERR_NONE;
}

error_t page_table_destroy(page_table_entry_t* page_table) {
	if (page_table->flags & PTEF_VALID)
		return ERR_NOT_VALID;
	delete_pte(*page_table);
	*page_table = (page_table_entry_t){0};
	return ERR_NONE;
}

error_t page_table_add_entry(page_table_entry_t* page_table, page_size_t ps, vpn_t vpn, page_table_entry_t entry) {
	u16 vpn_slice[5] = {
	    (vpn >> 9 * 0) & 0x1ff, (vpn >> 9 * 1) & 0x1ff, (vpn >> 9 * 2) & 0x1ff,
	    (vpn >> 9 * 3) & 0x1ff, (vpn >> 9 * 4) & 0x1ff,
	};
#ifdef __LOG_TRACE__
	const char* page_size_prefix[] = {"kilo", "mega", "giga", "tera", "peta"};
	KLOGLN_TRACE("Adding a %sframe #%lx to page table with root ppn: #%lx.", page_size_prefix[ps], entry.ppn,
	             page_table->ppn);
#endif
	KLOG_INDENT_BLOCK_START;
	u64* current_page = physical_to_effective(page_table->ppn << 12);
	u8 pt_height = 0;
	buffer_t pth_buff = kernel_config_get(KERCFG_PT_HEIGHT);
	if (pth_buff.error) {
		KLOGLN_TRACE("Failed to read kernel config.");
		KLOG_END_BLOCK_AND_RETURN(ERR_INTERNAL_FAILURE);
	}
	error_t err = buffer_read_u8(pth_buff, 0, &pt_height);
	if (err) {
		KLOGLN_TRACE("Failed to read kernel config.");
		KLOG_END_BLOCK_AND_RETURN(ERR_INTERNAL_FAILURE);
	}
	for (i32 lvl = pt_height - 1; lvl > ps; --lvl) {
		u64* current_riscv_pte = &(current_page[vpn_slice[lvl]]);
		page_table_entry_t current_pte = read_riscv_pte(*current_riscv_pte);
		if ((current_pte.flags & PTEF_VALID) == 0) {
			ppn_t new_ppn = 0;
			error_t err = phys_mem_alloc_frame(PAGE_SIZE_4kB, &new_ppn);
			if (err) {
				KLOGLN_TRACE("Failed to allocate a frame.");
				KLOG_END_BLOCK_AND_RETURN(err);
			}
			current_pte.ppn = new_ppn;
			memset(physical_to_effective(current_pte.ppn << 12), 0, 0x1000);
			current_pte.flags = PTEF_VALID;
			current_pte.flags |= entry.flags & (PTEF_GLOBAL | PTEF_USER);
			KLOGLN_TRACE("Adding a pointer frame of ppn: #%lx...", new_ppn);
			*current_riscv_pte = write_riscv_pte(current_pte);
		}
		current_page = physical_to_effective(current_pte.ppn << 12);
	}
	page_table_entry_t target_pte = read_riscv_pte(current_page[vpn_slice[ps]]);
	if (target_pte.flags & PTEF_VALID)
		KLOG_END_BLOCK_AND_RETURN(ERR_NOT_VALID);
	KLOGLN_TRACE("Adding a target frame of ppn: #%lx...", entry.ppn);
	current_page[vpn_slice[ps]] = write_riscv_pte(entry);
	KLOG_END_BLOCK_AND_RETURN(ERR_NONE);
}

error_t page_table_remove_region(page_table_entry_t* root_pte, virt_mem_region_t region) {
	return ERR_NOT_IMPLEMENTED;
}

error_t page_table_walk(page_table_entry_t* page_table, void* vaddr, phys_addr_t* paddrOUT) {
	vpn_t vpn = (u64)vaddr >> 12;
	u16 vpn_slice[5] = {
	    (vpn >> 9 * 0) & 0x1ff, (vpn >> 9 * 1) & 0x1ff, (vpn >> 9 * 2) & 0x1ff,
	    (vpn >> 9 * 3) & 0x1ff, (vpn >> 9 * 4) & 0x1ff,
	};
	u8 pt_height = 0;
	{
		buffer_t pth_buffer = kernel_config_get(KERCFG_PT_HEIGHT);
		if (pth_buffer.error) {
			KLOGLN_TRACE("Failed to read kernel config.");
			return ERR_INTERNAL_FAILURE;
		}
		error_t err = buffer_read_u8(pth_buffer, 0, &pt_height);
		if (err) {
			KLOGLN_TRACE("Failed to read buffer.");
			return ERR_INTERNAL_FAILURE;
		}
	}
	if ((page_table->flags & PTEF_VALID) == 0)
		return ERR_NOT_VALID;
	ppn_t current_ppn = page_table->ppn;
	for (i32 lvl = pt_height - 1; lvl >= 0; --lvl) {
		const u64* riscv_page_table = physical_to_effective(current_ppn << 12);
		const u64 riscv_page_table_entry = riscv_page_table[vpn_slice[lvl]];
		page_table_entry_t pte = read_riscv_pte(riscv_page_table_entry);
		if ((pte.flags & PTEF_VALID) == 0)
			return ERR_NOT_FOUND;
		current_ppn = pte.ppn;
		if (is_pte_leaf(pte)) {
			u64 ret_addr = current_ppn << 12;
			const u64 addr_offset_mask = (1ull << (12 + 9 * lvl)) - 1;
			const u64 addr_offset = (u64)vaddr & addr_offset_mask;
			ret_addr &= ~addr_offset_mask;
			ret_addr |= addr_offset;
			*paddrOUT = ret_addr;
			return ERR_NONE;
		}
	}
	return ERR_BAD_ARG;                              // Should never happen
}

void page_table_print(page_table_entry_t root_pte) { // TODO: This is awfull and needs a rewrite
	buffer_t h_buff = {0};
	h_buff = kernel_config_get(KERCFG_PT_HEIGHT);
	if (h_buff.error) {
		DEBUG_PRINTF("This is not good\n");
		return;
	}
	u8 h = 0;
	const error_t err = buffer_read_u8(h_buff, 0, &h);
	if (err) {
		DEBUG_PRINTF("This is not good\n");
		return;
	}
	print_pte(root_pte, 0, h, 0);
}
