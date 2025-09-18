#include "virtual_memory_managment.h"

#include <stdbigos/csr.h>

// ========================================
//				  Private
// ========================================

typedef struct {
	ppn_t page_table;
	u16 asid;
	u8 vms;
} reg_satp_t;

reg_t write_satp(reg_satp_t satp) {
	reg_t out = (reg_t)satp.page_table & 0xfffffffffff;
	out |= ((reg_t)satp.asid & 0xffff) << 44;
	out |= ((reg_t)satp.vms & 0xf) << 60;
	return out;
}

reg_satp_t read_satp(reg_t satp) {
	reg_satp_t out = {0};
	out.page_table = satp & 0xfffffffffff;
	out.asid = (satp >> 44) & 0xffff;
	out.vms = (satp >> 60) & 0xf;
	return out;
}

// ========================================
//				  Public
// ========================================

u16 virt_mem_get_max_asid() {
	reg_satp_t satp_struct = {.vms = 0, .asid = UINT16_MAX, .page_table = 0};
	volatile reg_t satp = write_satp(satp_struct);
	CSR_WRITE(satp, satp);
	satp = CSR_READ(satp);
	satp_struct = read_satp(satp);
	return satp_struct.asid;
}

void virt_mem_set_satp(u16 asid, u8 vms, ppn_t page_table) {
	reg_satp_t satp_struct = {.vms = vms, .asid = asid, .page_table = page_table};
	volatile reg_t satp = write_satp(satp_struct);
	CSR_WRITE(satp, satp);
}

void virt_mem_flush_TLB() {
	asm volatile("sfence.vma x0, x0" ::: "memory");
}

void virt_mem_flush_TLB_entry(void* va) {
	register u64 r_va asm("a0") = (reg_t)va;
	asm volatile("sfence.vma %0, x0" ::"r"(r_va) : "memory");
}

void virt_mem_flush_TLB_address_space(u64 asid) {
	register u64 r_asid asm("a0") = (reg_t)asid;
	asm volatile("sfence.vma x0, %0" ::"r"(r_asid) : "memory");
}
