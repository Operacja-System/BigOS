#ifndef _KERNEL_VIRTUAL_MEMORY_PAGE_TABLE_H
#define _KERNEL_VIRTUAL_MEMORY_PAGE_TABLE_H

#include <stdbigos/error.h>
#include "mm_common.h"
#include "page_table_entry.h"

typedef struct [[gnu::packed]] {
	physical_page_number_t root_pte_ppn : 44;
	u8 tags;
	/* tags:
	 * bit 0: Valid
	 * bit 1: Saint
	 */
} page_table_t;

void page_table_init(page_table_t* pt, physical_page_number_t ppn, bool saint);
void page_table_delete(page_table_t* pt);
// NOTE: all arguments except "pt" can be nullptr, in that case they will be ignored
void get_page_table_tags(page_table_t* pt, bool* valid, bool* saint);
void page_table_add_entry(page_table_t* pt, virtual_page_number_t vpn, page_table_entry_t pte, page_size_t psize);


#endif //!_KERNEL_VIRTUAL_MEMORY_PAGE_TABLE_H
