#ifndef BIGOS_KERNEL_MEMORY_MANAGMENT_ADDRESS_SPACE_MANAGER
#define BIGOS_KERNEL_MEMORY_MANAGMENT_ADDRESS_SPACE_MANAGER

#include <stdbigos/error.h>
#include <stdbigos/types.h>

#include "mm_types.h"

typedef u32 asid_t;

typedef struct {
	bool user;
	bool global;
	bool valid;
	asid_t asid;
	u64 generation;
	vpn_t stack_top_page;
	vpn_t heap_top_page;
	ppn_t root_pte;
} as_handle_t;

[[nodiscard]] error_t address_space_managment_init(u16 max_asid);
[[nodiscard]] error_t address_space_create(as_handle_t* ashOUT, bool user, bool global);
[[nodiscard]] error_t address_space_destroy(as_handle_t* ash);
[[nodiscard]] error_t address_sapce_add_region(as_handle_t* ash, virt_mem_region_t region);
[[nodiscard]] error_t address_sapce_delete_region(as_handle_t* ash, virt_mem_region_t region);
[[nodiscard]] error_t address_space_resolve_page_fault(as_handle_t* ash, void* fault_addr, bool read, bool write,
                                                       bool execute);
[[nodiscard]] error_t address_space_set_stack_data(as_handle_t* ash, void* stack_start, size_t stack_size);
[[nodiscard]] error_t address_space_set_heap_data(as_handle_t* ash, void* heap_start, size_t heap_size);

#endif // !BIGOS_KERNEL_MEMORY_MANAGMENT_ADDRESS_SPACE_MANAGER
