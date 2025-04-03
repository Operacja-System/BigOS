#include "vmm.h"

#include <stdbigos/csr.h>
#include <stdbigos/string.h>

#include "kmalloc.h"
#include "stdbigos/error.h"
#include "virtual_memory/mm_common.h"
#include "virtual_memory/pmm.h"

#define ENSURE_VM_INITED                                                        \
	if(!g_virtual_memory_initialized) return ERR_VIRTUAL_MEMORY_NOT_INITIALIZED

//==========================================================================//
//===							   GLOBALS								 ===//
//==========================================================================//

static u16 g_asid_max_val = 0;
static virt_mem_scheme_t g_active_vms = 0;
static page_table_entry_t** g_page_table_table = nullptr;
static u64* g_page_table_table_vmask = nullptr;
static bool g_virtual_memory_initialized = false;
static void* g_RAM_map = nullptr;

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

//NOTE: if pte is nullptr this function has no effect
static void write_to_page_table_entry(page_table_entry_t* pte, u8 N, u8 PBMT, u64 PPN, u8 RSW, u8 permissions) {
	if(!pte) return;
	page_table_entry_t new_pte = 
	(N				& 0x1ull)			<< 63|
	(PBMT			& 0x3ull)			<< 61|
	(PPN			& 0xfffffffffffull) << 10|
	(RSW			& 0x3ull)			<< 8 |
	(permissions	& 0xffull)			<< 0 ;
	*pte = new_pte;
}

//NOTE: all arguments after "pte" can be nullptr (argument ignored)
static void read_page_table_entry(page_table_entry_t pte, u8* N, u8* PBMT, u64* PPN, u8* RSW, u8* permissions) {
	if(N) *N = pte >> 63;
	if(PBMT) *PBMT = pte >> 61 & 0x3ull;
	if(PPN) *PPN = pte >> 10 & 0xfffffffffffull;
	if(RSW) *RSW = pte >> 8 & 0x3ull;
	if(permissions) *permissions = pte & 0xffull;
}

// NOTE:!!! All the functions below assume that virtual memory has been initialized !!!
[[nodiscard]] static bool is_page_table_valid(asid_t asid) {
	u64 ptt_vmask_inx = (u64)asid >> 6ull;
	u64 ptt_vmask_offset = (u64)asid & 0x3full;
	return g_page_table_table_vmask[ptt_vmask_inx] & (1ull << ptt_vmask_offset);
}

static void validate_page_table(asid_t asid) {
	u64 ptt_vmask_inx = (u64)asid >> 6ull;
	u64 ptt_vmask_offset = (u64)asid & 0x3full;
	g_page_table_table_vmask[ptt_vmask_inx] |= (1ull << ptt_vmask_offset);
}

static void invalidate_page_table(asid_t asid) {
	u64 ptt_vmask_inx = (u64)asid >> 6ull;
	u64 ptt_vmask_offset = (u64)asid & 0x3full;
	g_page_table_table_vmask[ptt_vmask_inx] &= ~(1ull << ptt_vmask_offset);
}

[[nodiscard]] static inline void* physical_to_virtual(phys_addr_t paddr) {
	return g_RAM_map + paddr;
}

static void destroy_page_table_entry(page_table_entry_t* pte) {
	
}

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
	g_RAM_map = RAM_start;
	if(g_asid_max_val == 0) return ERR_ASID_NOT_SUPPORTED;
	CSR_WRITE(satp, 0);
	g_virtual_memory_initialized = true;
	error_t kmalloc_err = saint_kmalloc(g_asid_max_val * sizeof(page_table_entry_t), (void*)&g_page_table_table);
	// TODO: Deal with kmalloc error
	kmalloc_err = saint_kmalloc(((g_asid_max_val >> 6ull) + ((g_asid_max_val & 0x3full) != 0)) * sizeof(u64), (void*)&g_page_table_table_vmask);
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
	kfree(g_page_table_table_vmask);
	return ERR_NONE;
}

asid_t get_asid_max_val() {
	ENSURE_VM_INITED;
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

#define _4KB   0x1000ull
#define _2MB   0x200000ull
#define _1GB   0x40000000ull
#define _512GB 0x8000000000ull
#define _128TB 0x800000000000ull

error_t create_page_table(asid_t asid, bool saint) {
	ENSURE_VM_INITED;
	if(is_page_table_valid(asid)) return ERR_PAGE_TABLE_ALREADY_EXISTS;
	phys_addr_t root_PTE_paddr = 0;
	error_t pmm_err = alloc_frame(PAGE_SIZE_4K, &root_PTE_paddr);
	// TODO: handle pmm_err
	page_table_entry_t* root_PTE_vaddr = physical_to_virtual(root_PTE_paddr);
	memset(root_PTE_vaddr, 0, _4KB);
	// TODO: save information whether the page_table is a saint
	g_page_table_table[asid] = root_PTE_vaddr;
	validate_page_table(asid);
	return ERR_NONE;
}

error_t destroy_page_table(asid_t asid) {
	ENSURE_VM_INITED;
	if(!is_page_table_valid(asid)) return ERR_PAGE_TABLE_DOESNT_EXIST;


	return ERR_NONE;
}

error_t add_page_table_entry(asid_t asid, page_size_t page_size, virt_addr_t vaddr, phys_addr_t paddr, page_table_entry_perms_t perms) {
	ENSURE_VM_INITED;

	return ERR_NONE;
}

error_t get_page_table_entry(asid_t asid, virt_addr_t vaddr, page_table_entry_t** pteOUT) {
	ENSURE_VM_INITED;

	return ERR_NONE;
}

error_t remove_page_table_entry(asid_t asid, virt_addr_t vaddr) {
	ENSURE_VM_INITED;

	return ERR_NONE;
}

error_t resolve_page_fault() {
	ENSURE_VM_INITED;

	return ERR_NONE;
}

