#ifndef BIGOS_KERNEL_MEMORY_MANAGER_PAGE_TABLE
#define BIGOS_KERNEL_MEMORY_MANAGER_PAGE_TABLE

#include <stdbigos/error.h>
#include <stdbigos/types.h>

#include "mm_types.h"

typedef enum : u8 {
	PTEF_VALID = (1 << 0),
	PTEF_READ = (1 << 1),
	PTEF_WRITE = (1 << 2),
	PTEF_EXECUTE = (1 << 3),
	PTEF_USER = (1 << 4),
	PTEF_GLOBAL = (1 << 5),
	PTEF_ACCESED = (1 << 6),
	PTEF_DIRTY = (1 << 7),
} page_table_entry_flags_t;

typedef enum : u8 {
	PTER_PLACEHOLDER1 = (1 << 0),
	PTER_PLACEHOLDER2 = (1 << 1),
} page_table_entry_os_flags_t;

typedef struct [[gnu::packed]] {
	u8 N : 1;    // Reserved by extenction svnapot. Must be 0 if the extenction is not used
	u8 pbmt : 2; // Reserved by extenction svpbmt. Must be 0 if the extenction is not used
	ppn_t ppn : 44;
	page_table_entry_os_flags_t os_flags : 2;
	page_table_entry_flags_t flags : 8;
} page_table_entry_t;
static_assert(sizeof(page_table_entry_t) == 8);

[[nodiscard]] error_t page_table_create(page_table_entry_t* page_tableOUT);
[[nodiscard]] error_t page_table_destroy(page_table_entry_t* page_table);
[[nodiscard]] error_t page_table_add_entry(page_table_entry_t* page_table, page_size_t ps, vpn_t vpn, page_table_entry_t entry);
void page_table_print(page_table_entry_t root_pte);

#endif // !BIGOS_KERNEL_MEMORY_MANAGER_PAGE_TABLE
