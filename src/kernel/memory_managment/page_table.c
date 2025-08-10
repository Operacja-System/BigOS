#include "page_table.h"

#include <debug/debug_stdio.h>
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>

#include "kernel_config.h"
#include "memory_managment/physical_memory_manager.h"
#include "ram_map.h"

// ========================================
//				  Private
// ========================================

static constexpr u32 pt_entries_amount = 512;

static page_table_entry_t read_riscv_pte(u64 rv_pte) {
	u8 flags = rv_pte & 0xff;
	u8 rsw = (rv_pte >> 8) & 0b11;
	ppn_t ppn = (rv_pte >> 10) & 0xfffffffffff;
	u8 pbmt = (rv_pte >> 60) & 0b11;
	u8 N = (rv_pte >> 62) & 0b1;
	return (page_table_entry_t){.flags = flags, .N = N, .ppn = ppn, .pbmt = pbmt, .os_flags = rsw};
}

static u64 write_riscv_pte(page_table_entry_t pte) {
	u64 rv_pte = 0;
	rv_pte |= (u64)pte.flags & 0xff;
	rv_pte |= (u64)(pte.os_flags & 0b11) << 8;
	rv_pte |= (u64)(pte.ppn & 0xfffffffffff) << 10;
	rv_pte |= (u64)(pte.pbmt & 0b11) << 61;
	rv_pte |= (u64)(pte.N & 0b1) << 63;
	return rv_pte;
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

void print_pte(page_table_entry_t pte, u8 lvl, u64 virt_addr) {
	if (!(pte.flags & PTEF_VALID))
		return;
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
		if (pte.flags & PTEF_ACCESED)
			flags[1] = 'A';
		if (pte.flags & PTEF_DIRTY)
			flags[0] = 'D';
		const char* ps_prefix[5] = {
		    "kilo", "mega", "giga", "tera", "peta",
		};
		DEBUG_PRINTF("addr: %016lx - ps: %s_page - os_flags: %s - flags: %s\n", virt_addr, ps_prefix[lvl], os_flags,
		             flags);
		return;
	}
	ppn_t ppn = pte.ppn;
	u64(*current_page)[pt_entries_amount] = (u64(*)[pt_entries_amount])(ppn << 12);
	for (u32 i = 0; i < pt_entries_amount; ++i) {
		u64 rv_pte = (*current_page)[i];
		page_table_entry_t new_pte = read_riscv_pte(rv_pte);
		print_pte(new_pte, lvl + 1, (virt_addr << 9) | i);
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
	u64(*current_page)[pt_entries_amount] = (u64(*)[pt_entries_amount])((ppn_t)(page_table->ppn) << 12);
	u8 pt_height = 0;
	buffer_t pth_buff = kernel_config_get(KERCFG_PT_HEIGHT);
	if (pth_buff.error)
		return ERR_INTERNAL_FAILURE;
	error_t err = buffer_read_u8(pth_buff, 0, &pt_height);
	if (err)
		return ERR_INTERNAL_FAILURE;
	for (i32 lvl = pt_height - 1; lvl > ps; --lvl) {
		u64* current_rv_pte = &(*current_page)[vpn_slice[lvl]];
		page_table_entry_t current_pte = read_riscv_pte(*current_rv_pte);
		if ((current_pte.flags & PTEF_VALID) == 0) {
			ppn_t new_ppn = 0;
			error_t err = phys_mem_alloc_frame(PAGE_SIZE_4kB, &new_ppn);
			if (err)
				return err;
			current_pte.ppn = new_ppn;
			memset(physical_to_effective(current_pte.ppn << 12), 0, 0x1000);
			current_pte.flags = PTEF_VALID;
			current_pte.flags |= entry.flags & (PTEF_GLOBAL | PTEF_USER);
			*current_rv_pte = write_riscv_pte(current_pte);
		}
		current_page = (u64(*)[pt_entries_amount])(current_pte.ppn << 12);
	}
	(*current_page)[vpn_slice[ps]] = write_riscv_pte(entry);
	return ERR_NONE;
}

error_t page_table_remove_region(page_table_entry_t* root_pte, virt_mem_region_t region) {
	return ERR_NOT_IMPLEMENTED;
}

void page_table_print(page_table_entry_t root_pte) {
	print_pte(root_pte, 0, 0);
}
