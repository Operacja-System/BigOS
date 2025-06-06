#ifndef BIGOS_KERNEL_MEMORY_MANAGMENT_ADDRESS_SPACE_MANAGER
#define BIGOS_KERNEL_MEMORY_MANAGMENT_ADDRESS_SPACE_MANAGER

#include <stdbigos/error.h>
#include <stdbigos/types.h>

typedef u32 asid_t;

typedef enum {
	PAGE_MODE_NORMAL,
	PAGE_MODE_HUGE,
	PAGE_MODE_GIGANTIC,
} page_mode_t;

error_t address_space_managment_init();
error_t address_space_create(asid_t* asidOUT);
error_t address_space_destroy(asid_t asidOUT);
error_t address_space_resolve_page_fault(asid_t asid, void* fault_addr, bool read, bool write, bool execute);

#endif // !BIGOS_KERNEL_MEMORY_MANAGMENT_ADDRESS_SPACE_MANAGER
