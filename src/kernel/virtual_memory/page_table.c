#include "page_table.h"
#include <stdbigos/string.h>
#include "pmm.h"
#include "ram_map.h"
#include "vmm.h"

error_t page_table_create(page_table_t* ptOUT, bool global, bool user) {
	if(ptOUT == nullptr) return ERR_INVALID_ARGUMENT;
	page_table_entry_t new_page_table = (page_table_entry_t){
		.n = 0,
		.pbmt = 0,
		.reserved = 0,
		.rsw = 0,
		.flags = PTEF_V | ((global) ? PTEF_G : 0) | ((user) ? PTEF_U : 0),
		.ppn = 0,
	};
	ppn_t page_frame = 0;
	error_t alloc_err = allocate_page_frame(PAGE_SIZE_4kB, &page_frame);
	//TODO: handle alloc_err
	new_page_table.ppn = page_frame;
	const int _4kB = 0x1000;
	memset(get_effective_address(new_page_table.ppn, 0), 0, _4kB);
	*ptOUT = new_page_table;
	return ERR_NONE;
}

error_t page_table_destroy(page_table_t* pt) {
	if(!(pt->flags & PTEF_V)) return ERR_INVALID_ARGUMENT;
	page_table_entry_t (*pt_node)[512] = (page_table_entry_t(*)[512])get_effective_address(pt->ppn, 0);
	for(u16 i = 0; i < 512; ++i) {
		page_table_entry_t* curr_pte = &(*pt_node[i]);
		if(!(curr_pte->flags & PTEF_V)) continue;
		if(!(curr_pte->flags & PTEF_R) && !(curr_pte->flags & PTEF_W) && !(curr_pte->flags & PTEF_X)) { //pte is not a leaf
			(void)page_table_destroy(curr_pte);
		}
		else { //pte is a leaf
			free_page_frame(curr_pte->ppn);
			*curr_pte = (page_table_entry_t){0};
		}
	}
	free_page_frame(pt->ppn);
	*pt = (page_table_t){0};
	return ERR_NONE;
}

error_t page_table_add_entry(page_table_t pt, page_size_t ps, vpn_t vpn, ppn_t ppn) {

	return ERR_NONE;
}

error_t page_table_remove_entry(page_table_t pt, vpn_t vpn) {

	return ERR_NONE;
}
