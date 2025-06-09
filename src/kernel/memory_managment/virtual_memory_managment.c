#include "virtual_memory_managment.h"
#include <stdbigos/csr.h>

// ========================================
//				  Private
// ========================================

typedef struct [[gnu::packed]] {
	ppn_t page_table : 44;
	u16 asid : 16;
	virt_mem_scheme_t vms : 4;
} reg_satp_t;
static_assert(sizeof(reg_satp_t) == 8);

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
	reg_satp_t satp_struct = {.vms = VMS_BARE, .asid = UINT16_MAX, .page_table = 0};
	volatile reg_t satp = write_satp(satp_struct);
	CSR_WRITE(satp, satp);
	satp = CSR_READ(satp);
	satp_struct = read_satp(satp);
	return satp_struct.asid;
}

void virt_mem_set_satp(u16 asid, virt_mem_scheme_t vms, ppn_t page_table) {

}
