#ifndef _KERNEL_VIRTUAL_MEMORY_PAE_TABLE_H_
#define _KERNEL_VIRTUAL_MEMORY_PAE_TABLE_H_

#include <stdbigos/error.h>
#include <stdbigos/types.h>

#include "mm_common.h"

typedef u64 page_table_entry_t;
typedef page_table_entry_t page_table_t;

[[nodiscard]] u64 create_page_table_entry(page_table_entry_flags_t flags, u8 rsw, ppn_t ppn);
void read_page_table_entry(u64 pte, page_table_entry_flags_t* flagsOUT, u8* rswOUT, ppn_t* ppnOUT);

[[nodiscard]] error_t page_table_create(page_table_t* ptOUT, bool global, bool user);
[[nodiscard]] error_t page_table_destroy(page_table_t* pt);
[[nodiscard]] error_t page_table_add_entry(page_table_t pt, page_size_t ps, vpn_t vpn, ppn_t ppn, bool R, bool W,
										   bool X);
[[nodiscard]] error_t page_table_remove_entry(page_table_t pt, vpn_t vpn); // only removes leaf pages
[[nodiscard]] error_t page_table_get_entry(page_table_t pt, vpn_t vpn, page_table_entry_t* pteOUT);
[[nodiscard]] error_t page_table_get_entry_ref(page_table_t pt, vpn_t vpn, page_table_entry_t** pteOUT);

#endif // !_KERNEL_VIRTUAL_MEMORY_PAE_TABLE_H_
