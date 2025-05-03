#ifndef _KERNEL_VIRTUAL_MEMORY_PAE_TABLE_H_
#define _KERNEL_VIRTUAL_MEMORY_PAE_TABLE_H_

#include <stdbigos/error.h>
#include <stdbigos/types.h>
#include "mm_common.h"

typedef enum : u8 {
	PTEF_V = (1u << 0u),
	PTEF_R = (1u << 1u),
	PTEF_W = (1u << 2u),
	PTEF_X = (1u << 3u),
	PTEF_U = (1u << 4u),
	PTEF_G = (1u << 5u),
	PTEF_A = (1u << 6u),
	PTEF_D = (1u << 7u),
} page_table_entry_flags_t;

typedef struct [[gnu::packed]] {
	page_table_entry_flags_t flags : 8;
	u8 rsw : 2;
	u64 ppn : 44;
	u8 reserved : 7;
	u8 pbmt : 2;
	u8 n : 1;
} page_table_entry_t;
static_assert(sizeof(page_table_entry_t) == 8);

typedef page_table_entry_t page_table_t;

[[nodiscard]] error_t page_table_create(page_table_t* ptOUT, bool global, bool user);
[[nodiscard]] error_t page_table_destroy(page_table_t* pt);
[[nodiscard]] error_t page_table_add_entry(page_table_t pt, page_size_t ps, vpn_t vpn, ppn_t ppn);
[[nodiscard]] error_t page_table_remove_entry(page_table_t pt, vpn_t vpn); //only removes leaf pages

#endif // !_KERNEL_VIRTUAL_MEMORY_PAE_TABLE_H_
