#include "vmm.h"

#include <stdbigos/csr.h>

//==========================================================================//
//===							   GLOBALS								 ===//
//==========================================================================//

static u16 g_asid_max_val = 0;
static virt_mem_scheme_t g_active_vms = 0;
static asid_t* g_page_table_table = NULL; // TODO: This is an array of size "asid-max_val - 1" that should be saint_kmalloc-ed

//==========================================================================//
//===							   INTERNAL								 ===//
//==========================================================================//

static void TLB_flush() {
	__asm__ volatile("sfence.vma");
}

[[nodiscard]] static error_t validate_virt_mem_scheme(virt_mem_scheme_t vms) {
	switch(vms) {
	case VMS_BARE: return ERR_NONE;
	case VMS_Sv39: return ERR_VIRT_MEM_SCHEME_NOT_IMPLEMENTED;
	case VMS_Sv48: return ERR_NONE;
	case VMS_Sv57: return ERR_VIRT_MEM_SCHEME_NOT_IMPLEMENTED;
	case VMS_Sv64: return ERR_VIRT_MEM_SCHEME_NOT_IMPLEMENTED;
	}
	return ERR_NONE;
}

[[nodiscard]] static error_t validate_page_size(virt_mem_scheme_t vms, page_size_t psize) {
	switch(vms) {
	case VMS_BARE: return ERR_NONE;
	case VMS_Sv39:
		if(psize > PAGE_SIZE_1G) return ERR_INVALID_PAGE_SIZE;
		break;
	case VMS_Sv48:
		if(psize > PAGE_SIZE_512G) return ERR_INVALID_PAGE_SIZE;
		break;
	case VMS_Sv57:
		if(psize > PAGE_SIZE_256T) return ERR_INVALID_PAGE_SIZE;
		break;
	case VMS_Sv64: return ERR_INVALID_PAGE_SIZE; break; // NOTE: RISC-V ISA does not specify Sv64 yet (29-03-2025)
	}
	return ERR_NONE;
}

//==========================================================================//
//===							   PUBLIC								 ===//
//==========================================================================//

/*
 * satp structure: |MODE|ASID | PPN | (Phisical Page Number)
 * 64bit		   |4bit|16bit|44bit|
 */
error_t virtual_memory_init(virt_mem_scheme_t vms, asid_t asid) {
	error_t vms_err = validate_virt_mem_scheme(vms);
	if(vms_err) return vms_err;
	u64 asidlen_check = (u64)UINT16_MAX << 44u;
	CSR_WRITE(satp, asidlen_check);
	g_asid_max_val = CSR_READ(satp) >> 44u;
	if(asid > g_asid_max_val) return ERR_ASID_NOT_SUPPORTED;
	CSR_WRITE(satp, 0);
	u64 satp_val = 0;
	satp_val |= (u64)vms << 60u;
	satp_val |= (u64)asid << 44u;
	// TODO: satp |= page_tables[asid] (physical)
	CSR_WRITE(satp, satp_val);
	if(CSR_READ(satp) != satp_val) return ERR_VIRT_MEM_SCHEME_NOT_SUPPORTED;
	TLB_flush();
	g_active_vms = vms;
	return ERR_NONE;
}

asid_t get_asid_max_val() {
	return g_asid_max_val;
}

virt_mem_scheme_t get_active_virt_mem_scheme() {
	return g_active_vms;
}

const char* get_virt_mem_scheme_str_name(virt_mem_scheme_t vms) {
	switch(vms) {
	case VMS_BARE: return "Bear";
	case VMS_Sv39: return "Sv39";
	case VMS_Sv48: return "Sv48";
	case VMS_Sv57: return "Sv57";
	case VMS_Sv64: return "Sv64";
	}
	return "???";
}
