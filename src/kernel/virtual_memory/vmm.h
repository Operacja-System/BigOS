#ifndef _KERNEL_VIRTUAL_MEMORY_VMM_H_
#define _KERNEL_VIRTUAL_MEMORY_VMM_H_

#include <stdbigos/error.h>
#include <stdbigos/types.h>

#include "mm_common.h"

typedef u16 asid_t;

[[nodiscard]] error_t initialize_virtual_memory(virtual_memory_scheme_t vms /*TODO:, device tree*/);
[[nodiscard]] error_t enable_virtual_memory(asid_t asid);
[[nodiscard]] error_t create_address_space(page_size_t ps, bool global, bool user, asid_t* asidOUT);
[[nodiscard]] error_t destroy_address_space(asid_t asid);
[[nodiscard]] error_t resolve_page_fault(asid_t asid, void* failed_address);

virtual_memory_scheme_t get_active_virtual_memory_scheme();

#endif // !_KERNEL_VIRTUAL_MEMORY_VMM_H_
