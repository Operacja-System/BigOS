#include "vmm.h"

#include <stdbigos/csr.h>
#include <stdbigos/string.h>

#include "kmalloc.h"
#include "mm_common.h"
#include "pmm.h"
#include "stdbigos/error.h"
#include "virtual_memory/page_table.h"

#define ENSURE_VM_INITED                                  \
	if(!g_virtual_memory_initialized) [[clang::unlikely]] \
	return ERR_VIRTUAL_MEMORY_NOT_INITIALIZED

//==========================================================================//
//===							   TYPES								 ===//
//==========================================================================//

//==========================================================================//
//===							   GLOBALS								 ===//
//==========================================================================//

static u16 g_asid_max_val = 0;
static virt_mem_scheme_t g_active_vms = 0;
static page_table_t* g_page_table_table = nullptr;
static bool g_virtual_memory_initialized = false;

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

static void destroy_page_table_entry(page_table_entry_t* pte) {}

//==========================================================================//
//===							   VM META								 ===//
//==========================================================================//

/*
 * satp structure: |MODE|ASID | PPN | (Phisical Page Number)
 * 64bit		   |4bit|16bit|44bit|
 */

error_t virtual_memory_init(void* RAM_start) {
	u64 asidlen_check = (u64)UINT16_MAX << 44ull;
	CSR_WRITE(satp, asidlen_check);
	g_asid_max_val = CSR_READ(satp) >> 44ull;
	if(g_asid_max_val == 0) return ERR_ASID_NOT_SUPPORTED;
	CSR_WRITE(satp, 0);
	g_virtual_memory_initialized = true;
	error_t kmalloc_err = kmalloc(g_asid_max_val * sizeof(page_table_t), (void*)&g_page_table_table);
	// TODO: Deal with kmalloc error
	return ERR_NONE;
}

error_t virtual_memory_enable(virt_mem_scheme_t vms, asid_t asid) {
	ENSURE_VM_INITED;
	error_t vms_err = validate_virt_mem_scheme(vms);
	if(vms_err) return vms_err;
	if(asid > g_asid_max_val) return ERR_ASID_NOT_VALID;
	u64 satp_val = 0;
	satp_val |= (u64)vms << 60ull;
	satp_val |= (u64)asid << 44ull;
	// TODO: satp |= page_tables[asid] (physical)
	CSR_WRITE(satp, satp_val);
	if(CSR_READ(satp) != satp_val) return ERR_VIRT_MEM_SCHEME_NOT_SUPPORTED;
	TLB_flush();
	g_active_vms = vms;
	return ERR_NONE;
}

error_t virtual_memory_disable() {
	ENSURE_VM_INITED;
	CSR_WRITE(satp, 0);
	g_active_vms = 0;
	kfree(g_page_table_table);
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
	case VMS_BARE: return "Bare";
	case VMS_Sv39: return "Sv39";
	case VMS_Sv48: return "Sv48";
	case VMS_Sv57: return "Sv57";
	case VMS_Sv64: return "Sv64";
	}
	return "???";
}

//==========================================================================//
//===							VM MANAGMENT							 ===//
//==========================================================================//

error_t resolve_page_fault() {
	ENSURE_VM_INITED;

	return ERR_NONE;
}

