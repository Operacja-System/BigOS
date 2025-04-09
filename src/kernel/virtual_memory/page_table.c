#include "page_table.h"
#include "stdbigos/error.h"
#include "virtual_memory/mm_common.h"
#include "virtual_memory/page_table_entry.h"

void page_table_init(page_table_t* pt, physical_page_number_t ppn, bool saint, page_table_level_t ptl, bool user) {
	const bool valid = 1;
	pt->root_pte_ppn = ppn;
	pt->flags = 
		((u8)valid << 0) |
		((u8)saint << 1) |
		(((u8)ptl & 0x3u) << 2) |
		((u8)user << 4) ;
}

void page_table_delete(page_table_t* pt) {
	page_table_entry_t* root_pte = physical_to_virtual(pt->root_pte_ppn);
	destroy_page_table_entry_t(root_pte);
	pt->root_pte_ppn = 0;
	pt->flags = 0;
}

// NOTE: all arguments except "pt" can be nullptr, in that case they will be ignored
void get_page_table_tags(page_table_t* pt, bool* valid, bool* saint, page_table_level_t* ptl, bool* user) {
	if(valid) *valid = (pt->flags >> 0u) & 0x1u;
	if(saint) *saint = (pt->flags >> 1u) & 0x1u;
	if(ptl) *ptl = (pt->flags >> 2u) & 0x3u;
	if(user) *user = (pt->flags >> 4u) & 0x1u;
}

error_t page_table_add_entry(page_table_t* pt, virtual_page_number_t vpn, page_size_t psize, page_table_entry_perms_t perms) {
	page_table_level_t ptl = 0;
	bool user = perms & PTEP_U, pt_valid = false;
	get_page_table_tags(pt, &pt_valid, nullptr, &ptl, nullptr);
	if(!pt_valid) return ERR_INVALID_ARGUMENT;
	if(ptl == PTL_RESERVED) return ERR_INVALID_ARGUMENT;
	const u8 target_level = psize;
	const u16 vpn_parts[] = { (vpn >> 0u) & 0x1ffu, (vpn >> 9u) & 0x1ffu, (vpn >> 18u) & 0x1ffu, (vpn >> 27u) & 0x1ffu, (vpn >> 36u) & 0x1ffu };
	page_table_entry_t* pt_node = physical_to_virtual(pt->root_pte_ppn);
	for(u8 vpn_inx = (u8)ptl + 2; vpn_inx > target_level; --vpn_inx) {
		page_table_entry_t* cur_pte = &pt_node[vpn_parts[vpn_inx]];
		bool pV = false, pX = false, pW = false, pR = false, pU = false;
		physical_page_number_t pPPN = 0;
		u8 pFlags = 0;
		read_page_table_entry(*cur_pte, nullptr, nullptr, &pPPN, nullptr, &pFlags);
		read_flags(pFlags, nullptr, nullptr, nullptr, &pU, &pX, &pW, &pR, &pV);
		if(!pU && user) return ERR_PERMISSION_VIOLATION;
		if(!pV) {
			u8 flags = create_flags(0, 0, 0, user, 0, 0, 0, 1);
			page_table_entry_t new_pte = create_page_table_entry(0, 0, 0, flags, PAGE_SIZE_4K);
			*cur_pte = new_pte;
			read_page_table_entry(*cur_pte, nullptr, nullptr, &pPPN, nullptr, nullptr);
			pt_node = physical_to_virtual(pPPN);
		}
		else if(pX || pW || pR) return ERR_VADDR_IN_USE;
		else {
			pt_node = physical_to_virtual(pPPN);
		}
	}
	u8 pFlags = 0;
	bool pV = false;
	page_table_entry_t* cur_pte = &pt_node[vpn_parts[target_level]];
	read_page_table_entry(*cur_pte, nullptr, nullptr, nullptr, nullptr, &pFlags);
	read_flags(pFlags, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &pV);
	if(pV) return ERR_VADDR_IN_USE;
	u8 flags = create_flags(0, 0, perms & PTEP_G, perms & PTEP_U, perms & PTEP_X, perms & PTEP_W, perms & PTEP_R, 1);
	page_table_entry_t new_pte = create_page_table_entry(0, 0, 0, flags, psize);
	*cur_pte = new_pte;
	return ERR_NONE;
}

error_t page_table_get_entry(page_size_t* pt, virtual_page_number_t vpn, page_table_entry_t* pteOUT) {
	page_table_entry_t* pte = 0;
	error_t err = page_table_get_entry_reference(pt, vpn, &pte);
	*pteOUT = *pte;
	return err;
}

error_t page_table_get_entry_reference(page_size_t* pt, virtual_page_number_t vpn, page_table_entry_t** pteOUT) {
	//TODO:
	return ERR_NOT_IMPLEMENTED;
}

