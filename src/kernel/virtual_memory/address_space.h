#ifndef _KERNEL_VIRTUAL_MEMORY_ADDRESS_SPACE_H_
#define _KERNEL_VIRTUAL_MEMORY_ADDRESS_SPACE_H_

#include "mm_common.h"
#include <stdbigos/error.h>

typedef struct {
	ppn_t page_table_padr : 44;
	page_size_t page_size : 3;
	u8 global_bit : 1;
	u8 user_bit : 1;
	u8 valid_bit : 1;
} address_space_t;
static_assert(sizeof(address_space_t) == 8);

address_space_t address_space_create(page_size_t ps, bool global, bool user);
[[nodiscard]] error_t address_space_destroy(address_space_t as);
[[nodiscard]] error_t address_space_resolve_page_fault(address_space_t as, void* failed_address);

#endif // !_KERNEL_VIRTUAL_MEMORY_ADDRESS_SPACE_H_
