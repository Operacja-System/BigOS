#include "page_table.h"
#include "kernel_config.h"
#include "memory_managment/physical_memory_manager.h"
#include "ram_map.h"
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>

// Private

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
	return (pte.flags & PTEF_READ || pte.flags & PTEF_WRITE || pte.flags & PTEF_EXECUTE) == 0;
}

static void delete_pte(page_table_entry_t pte) {
	if(is_pte_leaf(pte)) {
		phys_mem_free_frame(pte.ppn);
		return;
	}
	u64(*pt)[pt_entries_amount] = physical_to_effective(pte.ppn << 12);
	for(u32 i = 0; i < pt_entries_amount; ++i) {
		page_table_entry_t new_pte = read_riscv_pte((*pt)[i]);
		if(new_pte.flags & PTEF_VALID)
			delete_pte(new_pte);
	}
	phys_mem_free_frame(pte.ppn);
}

static error_t add_entry(u64 root_pt_ppn, u8 pt_height, page_size_t ps, vpn_t vpn, ppn_t ppn, bool R, bool W, bool X, bool G, bool U) {
	u16 vpn_slice[5] = {
		(vpn >> 9 * 0) & 0x1ff,
		(vpn >> 9 * 1) & 0x1ff,
		(vpn >> 9 * 2) & 0x1ff,
		(vpn >> 9 * 3) & 0x1ff,
		(vpn >> 9 * 4) & 0x1ff,
	};
	u64(*current_page)[pt_entries_amount] = (u64(*)[pt_entries_amount])(root_pt_ppn << 12);
	for(i8 lvl = pt_height - 1; lvl > ps; --lvl) {
		u64* current_rv_pte = &(*current_page)[vpn_slice[lvl]];
		page_table_entry_t current_pte = read_riscv_pte(*current_rv_pte);
		if((current_pte.flags & PTEF_VALID) == 0) {
			error_t err = phys_mem_alloc_frame(PAGE_SIZE_4kB, &current_pte.ppn);
			memset((void*)(current_pte.ppn << 12), 0, 0x1000);
			current_pte.flags = PTEF_VALID;
			if(G) current_pte.flags |= PTEF_GLOBAL;
			if(U) current_pte.flags |= PTEF_USER;
			*current_rv_pte = write_riscv_pte(current_pte);
		}
		current_page = (u64(*)[pt_entries_amount])(current_pte.ppn << 12);
	}
	page_table_entry_t pte = {0};
	if(R) pte.flags |= PTEF_READ;
	if(W) pte.flags |= PTEF_WRITE;
	if(X) pte.flags |= PTEF_EXECUTE;
	if(G) pte.flags |= PTEF_GLOBAL;
	if(U) pte.flags |= PTEF_USER;
	pte.ppn = ppn;
	pte.os_flags = 0;
	pte.N = 0;
	pte.pbmt = 0;
	(*current_page)[vpn_slice[ps]] = write_riscv_pte(pte);
}

// Public

error_t page_table_create(page_table_entry_t* page_tableOUT) {
	*page_tableOUT = (page_table_entry_t){0};
	return ERR_NONE;
}

error_t page_table_destroy(page_table_entry_t* page_table) {
	if(page_table->flags & PTEF_VALID) return ERR_NOT_VALID;
	delete_pte(*page_table);
	*page_table = (page_table_entry_t){0};
	return ERR_NONE;
}

error_t page_table_add_region(page_table_entry_t* root_pte, virt_mem_region_t region) {
	if((root_pte->flags & PTEF_VALID) == 0) {
		u8 flags = PTEF_VALID | PTEF_GLOBAL;
		ppn_t ppn = 0;
		error_t err = phys_mem_alloc_frame(PAGE_SIZE_4kB, &ppn);
		if(err) return err;
		void* page_table_page = physical_to_effective(ppn << 12);
		memset(page_table_page, 0, 0x1000);
		page_table_entry_t new_root_pte = (page_table_entry_t){.flags = flags, .ppn = ppn, .os_flags = 0, .N = 0, .pbmt = 0};
		*root_pte = new_root_pte;
	}
	void* curr_addr = region.start;
	size_t size_left = region.size;
	phys_addr_t curr_map_addr = region.map_region.start;
	while(size_left > 0) {
		add_entry(*root_pte, u8 pt_height, region.ps, (u64)curr_addr >> 12, ppn_t ppn, bool R, bool W, bool X, bool G, bool U)
	}
}

error_t page_table_remove_region(page_table_entry_t* root_pte, virt_mem_region_t region) {
	return ERR_NOT_IMPLEMENTED;
}
