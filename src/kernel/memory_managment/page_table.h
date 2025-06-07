#ifndef BIGOS_KERNEL_MEMORY_MANAGER_PAGE_TABLE
#define BIGOS_KERNEL_MEMORY_MANAGER_PAGE_TABLE

#include <stdbigos/error.h>

#include "memory_managment/physical_memory_manager.h"

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

typedef struct {
	bool mapped;
	bool read;
	bool write;
	bool execute;
	page_size_t ps;
	void* start;
	size_t size;
	phys_mem_region_t map_region;
} virt_mem_region_t;

error_t page_table_create(page_table_entry_t* page_tableOUT);
error_t page_table_destroy(page_table_entry_t* page_table);
error_t page_table_add_region(page_table_entry_t* root_pte, virt_mem_region_t region);
error_t page_table_remove_region(page_table_entry_t* root_pte, virt_mem_region_t region);

#endif // !BIGOS_KERNEL_MEMORY_MANAGER_PAGE_TABLE
