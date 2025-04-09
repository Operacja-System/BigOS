#include "page_table.h"

void page_table_init(page_table_t* pt, physical_page_number_t ppn, bool saint) {
	const bool valid = 1;
	pt->root_pte_ppn = ppn;
	pt->tags = ((u8)valid << 0) | ((u8)saint << 1);
}

void page_table_delete(page_table_t* pt) {
	pt->root_pte_ppn = 0;
	pt->tags = 0;
}

// NOTE: all arguments except "pt" can be nullptr, in that case they will be ignored
void get_page_table_tags(page_table_t* pt, bool* valid, bool* saint) {
	if(valid) *valid = (pt->tags >> 0) & 1;
	if(saint) *saint = (pt->tags >> 1) & 1;
}

void page_table_add_entry(page_table_t* pt, virtual_page_number_t vpn, page_table_entry_t pte, page_size_t psize) {

}
