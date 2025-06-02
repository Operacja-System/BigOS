#include "page_table.h"

#include <stdbigos/string.h>

#include "pmm.h"
#include "ram_map.h"
#include "virtual_memory/mm_common.h"
#include "vmm.h"

page_table_entry_t create_page_table_entry(page_table_entry_flags_t flags, u8 rsw, ppn_t ppn) {
	return ((u64)flags & 0xff) | (((u64)rsw << 8) & 0x3) | (((u64)ppn << 10) & 0xfffffffffff);
}

void read_page_table_entry(page_table_entry_t pte, page_table_entry_flags_t* flagsOUT, u8* rswOUT, ppn_t* ppnOUT) {
	if(flagsOUT) *flagsOUT = pte & 0xff;
	if(rswOUT) *rswOUT = (pte >> 8) & 0x3;
	if(ppnOUT) *ppnOUT = (pte >> 10) & 0xfffffffffff;
}

bool is_pte_leaf(page_table_entry_t pte) {
	page_table_entry_flags_t flags = 0;
	read_page_table_entry(pte, &flags, nullptr, nullptr);
	return !(!(flags & PTEF_R) && !(flags & PTEF_W) && !(flags & PTEF_X));
}

error_t page_table_create(page_table_t* ptOUT, bool global, bool user) {
	if(ptOUT == nullptr) return ERR_INVALID_ARGUMENT;
	ppn_t page_frame = 0;
	error_t alloc_err = allocate_page_frame(PAGE_SIZE_4kB, &page_frame);
	// TODO: handle alloc_err
	const int _4kB = 0x1000;
	memset(get_effective_address(page_frame, 0), 0, _4kB);
	*ptOUT = create_page_table_entry(PTEF_V | ((global) ? PTEF_G : 0) | ((user) ? PTEF_U : 0), 0, page_frame);
	return ERR_NONE;
}

error_t page_table_destroy(page_table_t* pt) {
	ppn_t ppn = 0;
	page_table_entry_flags_t flags = 0;
	read_page_table_entry(*pt, &flags, nullptr, &ppn);
	if(!(flags & PTEF_V)) return ERR_INVALID_ARGUMENT;
	page_table_entry_t(*pt_node)[512] = (page_table_entry_t(*)[512])get_effective_address(ppn, 0);
	for(u16 i = 0; i < 512; ++i) {
		page_table_entry_t* curr_pte = &(*pt_node[i]);
		if(is_pte_leaf(*curr_pte)) {
			ppn_t curr_ppn = 0;
			page_table_entry_flags_t curr_flags = 0;
			read_page_table_entry(curr_ppn, &curr_flags, nullptr, &curr_ppn);
			if(!(curr_flags & PTEF_V)) continue;
			if(free_page_frame(curr_ppn)) return ERR_CRITICAL_INTERNAL_FAILURE;
			*curr_pte = (page_table_entry_t){0};
		} else {
			(void)page_table_destroy(curr_pte);
		}
	}
	if(free_page_frame(ppn)) return ERR_CRITICAL_INTERNAL_FAILURE;
	*pt = (page_table_t){0};
	return ERR_NONE;
}

error_t page_table_add_entry(page_table_t pt, page_size_t ps, vpn_t vpn, ppn_t ppn, bool R, bool W, bool X) {
	page_table_entry_flags_t flags = 0;
	read_page_table_entry(pt, &flags, nullptr, nullptr);
	const virtual_memory_scheme_t active_vms = get_active_virtual_memory_scheme();
	if(!R && !X) return ERR_INVALID_ARGUMENT; // Page cannot be --- or -w-
	const u16 vpn_slice[5] = {
		(vpn >> (9 * 0)) & 0x1ffu, (vpn >> (9 * 1)) & 0x1ffu, (vpn >> (9 * 2)) & 0x1ffu,
		(vpn >> (9 * 3)) & 0x1ffu, (vpn >> (9 * 4)) & 0x1ffu,
	};
	page_table_entry_t* curr_pte = &pt;
	for(i8 level = active_vms + 2; level >= ps; --level) {
		ppn_t curr_ppn = 0;
		page_table_entry_flags_t curr_flags = 0;
		read_page_table_entry(*curr_pte, &curr_flags, nullptr, &curr_ppn);
		curr_pte = &(*(page_table_entry_t(*)[512])get_effective_address(curr_ppn, 0))[vpn_slice[level]];
		if(curr_flags & PTEF_V) {
			if(is_pte_leaf(*curr_pte)) return ERR_INVALID_ARGUMENT;
			continue;
		}
		ppn_t new_ppn = 0;
		const error_t alloc_err = allocate_page_frame((level == ps) ? ps : PAGE_SIZE_4kB, &new_ppn);
		// TODO: handle alloc_err
		page_table_entry_flags_t access_perms = 0;
		if(level == ps) {
			if(R) access_perms |= PTEF_R;
			if(W) access_perms |= PTEF_W;
			if(X) access_perms |= PTEF_X;
		}
		*curr_pte = create_page_table_entry(PTEF_V | (flags & (PTEF_G | PTEF_U)) | access_perms, 0, new_ppn);
	}

	return ERR_NONE;
}

error_t page_table_get_entry_ref(page_table_t pt, vpn_t vpn, page_table_entry_t** pteOUT) {
	const u16 vpn_slice[5] = {
		(vpn >> (9 * 0)) & 0x1ffu, (vpn >> (9 * 1)) & 0x1ffu, (vpn >> (9 * 2)) & 0x1ffu,
		(vpn >> (9 * 3)) & 0x1ffu, (vpn >> (9 * 4)) & 0x1ffu,
	};
	page_table_entry_t* curr_pte = &pt;
	i8 level = get_active_virtual_memory_scheme() + 2;
	while(!is_pte_leaf(*curr_pte)) {
		ppn_t curr_ppn = 0;
		page_table_entry_flags_t curr_flags = 0;
		read_page_table_entry(*curr_pte, &curr_flags, nullptr, &curr_ppn);
		if(level < 0) return ERR_INVALID_ARGUMENT;
		curr_pte = &(*(page_table_entry_t(*)[512])get_effective_address(curr_ppn, 0))[vpn_slice[level]];
		if(!(curr_flags & PTEF_V)) return ERR_INVALID_ARGUMENT; // PTE is not mapped
		--level;
	}
	*pteOUT = curr_pte;
	return ERR_NONE;
}

error_t page_table_get_entry(page_table_t pt, vpn_t vpn, page_table_entry_t* pteOUT) {
	page_table_entry_t* pte = nullptr;
	const error_t err = page_table_get_entry_ref(pt, vpn, &pte);
	if(err) return err;
	*pteOUT = *pte;
	return ERR_NONE;
}

error_t page_table_remove_entry(page_table_t pt, vpn_t vpn) {
	page_table_entry_t* pte = nullptr;
	const error_t err = page_table_get_entry_ref(pt, vpn, &pte);
	if(err) return err;
	ppn_t ppn = 0;
	read_page_table_entry(*pte, nullptr, nullptr, &ppn);
	if(free_page_frame(ppn)) return ERR_CRITICAL_INTERNAL_FAILURE;
	*pte = (page_table_entry_t){0};
	return ERR_NONE;
}
