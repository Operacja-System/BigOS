#include "vmm.h"

#include <stdbigos/csr.h>

typedef u64 page_table_entry_t;

static u16 asid_max_val = 0;
static virt_mem_scheme_t vms_in_use = 0;

static void TLB_flush() {
	__asm__ volatile("sfence.vma");
}

static error_t validate_VMS(virt_mem_scheme_t vms) {
	switch(vms) {
	case VMS_DISABLE: return ERR_NONE;
	case VMS_Sv39:	  return ERR_VIRT_MEM_SCHEME_NOT_IMPLEMENTED;
	case VMS_Sv48:	  return ERR_NONE;
	case VMS_Sv57:	  return ERR_VIRT_MEM_SCHEME_NOT_IMPLEMENTED;
	case VMS_Sv64:	  return ERR_VIRT_MEM_SCHEME_NOT_IMPLEMENTED;
	}
}

/*
 * satp structure: |MODE|ASID | PPN |
 * 64bit		   |4bit|16bit|44bit|
 */
error_t virtual_memory_init(virt_mem_scheme_t vms, asid_t asid) {
	error_t vms_err = validate_VMS(vms);
	if(vms_err) return vms_err;
	vms_in_use = vms;
	u64 asidlen_check = (u64)UINT16_MAX << 44u;
	CSR_WRITE(satp, asidlen_check);
	asid_max_val = CSR_READ(satp) >> 44u;
	if(asid > asid_max_val) return ERR_ASID_NOT_SUPPORTED;
	CSR_WRITE(satp, 0);
	u64 satp_val = 0;
	satp_val |= (u64)vms << 60u;
	satp_val |= (u64)asid << 44u;
	// TODO: satp |= page_tables[asid] (physical)
	CSR_WRITE(satp, satp_val);
	if(CSR_READ(satp) != satp_val) return ERR_VIRT_MEM_SCHEME_NOT_SUPPORTED;
	TLB_flush();
	return ERR_NONE;
}

asid_t get_asid_max_val() {
	return asid_max_val;
}
