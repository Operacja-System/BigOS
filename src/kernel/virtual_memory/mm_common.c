#include "mm_common.h"

static virt_addr_t g_RAM_start = nullptr;

static virt_mem_scheme_t g_active_mem_scheme = VMS_BARE;

virt_addr_t physical_to_virtual(phys_addr_t paddr) {
	return g_RAM_start + paddr;
}
