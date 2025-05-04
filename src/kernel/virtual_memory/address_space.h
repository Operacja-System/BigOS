#ifndef _KERNEL_VIRTUAL_MEMORY_ADDRESS_SPACE_H_
#define _KERNEL_VIRTUAL_MEMORY_ADDRESS_SPACE_H_

#include <stdbigos/error.h>

#include "mm_common.h"
#include "page_table.h"

typedef struct {
	page_table_t page_table;
	page_size_t page_size : 3;
	u8 global_bit : 1;
	u8 user_bit : 1;
	u8 valid_bit : 1;
} address_space_t;

[[nodiscard]] error_t address_space_create(page_size_t ps, bool global, bool user, address_space_t* asOUT);
[[nodiscard]] error_t address_space_destroy(address_space_t* as);
[[nodiscard]] error_t address_space_handle_page_fault(address_space_t as, void* failed_address, bool R, bool W, bool X);

#endif // !_KERNEL_VIRTUAL_MEMORY_ADDRESS_SPACE_H_
