#ifndef _KERNEL_VIRTUAL_MEMORY_PAGE_TABLE_H
#define _KERNEL_VIRTUAL_MEMORY_PAGE_TABLE_H

#include <stdbigos/error.h>

#include "mm_common.h"
#include "page_table_entry.h"

typedef struct [[gnu::packed]] {
	physical_page_number_t root_pte_ppn : 44;
	u8 flags;
	/* flags:
	 * bit 0: Valid
	 * bit 1: Saint
	 * bits 2-3: Levels (0 - 3lvl, 1 - 4lvl, 2 - 5lvl, 3 - RESERVED)
	 * bit 4: User
	 */
} page_table_t;

typedef enum {
	PTL_3 = 0,
	PTL_4 = 1,
	PTL_5 = 2,
	PTL_RESERVED = 3, // NOTE: This should be updated when Sv64 is released
} page_table_level_t; // this field is 2 bits wide, only 4 unique values possible

void page_table_init(page_table_t* pt, physical_page_number_t ppn, bool saint, page_table_level_t ptl, bool user);
void page_table_delete(page_table_t* pt);
// all arguments except "pt" can be nullptr, in that case they will be ignored
void get_page_table_tags(page_table_t* pt, bool* valid, bool* saint, page_table_level_t* ptl, bool* user);
[[nodiscard]] error_t page_table_add_entry(page_table_t* pt, virtual_page_number_t vpn, page_size_t psize, page_table_entry_perms_t perms);
[[nodiscard]] error_t page_table_get_entry(page_size_t* pt, virtual_page_number_t vpn, page_table_entry_t* pteOUT);
[[nodiscard]] error_t page_table_get_entry_reference(page_size_t* pt, virtual_page_number_t vpn, page_table_entry_t** pteOUT);

#endif //!_KERNEL_VIRTUAL_MEMORY_PAGE_TABLE_H
