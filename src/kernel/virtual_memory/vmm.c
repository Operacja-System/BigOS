#include "vmm.h"
#include <stdbigos/csr.h>

typedef struct {

} page_table_entry_t;

ERROR virtual_memory_init(VIRT_MEM_SCHEME_t vm_scheme, asid_t asid) {
	if(vm_scheme != VMS_Sv48) return ERR_VIRT_MEM_SCHEME_NOT_SUPPORTED;
	u64 satp_val = 0;
	satp_val |= (u64)vm_scheme << 60;
	satp_val |= (u64)asid << 44;
	//TODO: satp |= page_tables[asid] (physical)
	CSR_SET(satp, satp_val);
	return ERR_NONE;
}
