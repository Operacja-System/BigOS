#include "page_table.h"
#include "kernel_config.h"

// Private

page_table_entry_t read_riscv_pte(u64 rv_pte) {
	u8 flags = rv_pte & 0xff;
	u8 rsw = (rv_pte >> 8) & 0b11;
	ppn_t ppn = (rv_pte >> 10) & 0xfffffffffff;
	u8 pbmt = (rv_pte >> 60) & 0b11;
	u8 N = (rv_pte >> 62) & 0b1;
	return (page_table_entry_t){.flags = flags, .N = N, .ppn = ppn, .pbmt = pbmt, .os_flags = rsw};
}

u64 write_riscv_pte(page_table_entry_t pte) {
	u64 rv_pte = 0;
	rv_pte |= (u64)pte.flags & 0xff;
	rv_pte |= (u64)(pte.os_flags & 0b11) << 8;
	rv_pte |= (u64)(pte.ppn & 0xfffffffffff) << 10;
	rv_pte |= (u64)(pte.pbmt & 0b11) << 61;
	rv_pte |= (u64)(pte.N & 0b1) << 63;
	return rv_pte;
}

bool is_pte_leaf(page_table_entry_t pte) {
	return (pte.flags & PTEF_READ || pte.flags & PTEF_WRITE || pte.flags & PTEF_EXECUTE) == 0;
}

// Public

error_t page_table_create(page_table_entry_t* page_tableOUT) {
	*page_tableOUT = (page_table_entry_t){0};
	return ERR_NONE;
}

error_t page_table_destroy(page_table_entry_t* page_table) {
	if(page_table->flags & PTEF_VALID) return ERR_NOT_VALID;
	u8 pt_height = bufferkernel_config_get(KERCFG_PT_HEIGHT);
}

error_t page_table_add_region(page_table_entry_t* root_pte, virt_mem_region_t region) {

}

error_t page_table_remove_region(page_table_entry_t* root_pte, virt_mem_region_t region) {
	return ERR_NOT_IMPLEMENTED;
}
