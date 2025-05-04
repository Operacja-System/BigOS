#include "address_space.h"

#include "virtual_memory/page_table.h"
#include "virtual_memory/pmm.h"
#include "virtual_memory/vmm.h"

error_t address_space_create(page_size_t ps, bool global, bool user, address_space_t* asOUT) {
	if((u8)ps > (u8)(get_active_virtual_memory_scheme() + 2)) return ERR_INVALID_ARGUMENT;
	page_table_t new_pt = {0};
	const error_t err = page_table_create(&new_pt, global, user);
	if(err) {
		*asOUT = (address_space_t){0};
		return err;
	}
	address_space_t new_address_space = (address_space_t){
		.user_bit = user,
		.global_bit = global,
		.valid_bit = true,
		.page_size = ps,
		.page_table = new_pt,
	};
	*asOUT = new_address_space;
	return ERR_NONE;
}

error_t address_space_destroy(address_space_t* as) {
	if(as->valid_bit == false) return ERR_INVALID_ARGUMENT;
	const error_t err = page_table_destroy(&as->page_table);
	if(err) return ERR_CRITICAL_INTERNAL_FAILURE;
	*as = (address_space_t){0};
	return ERR_NONE;
}

error_t address_space_handle_page_fault(address_space_t as, void* failed_address, bool R, bool W, bool X) {
	if(as.valid_bit == false) return ERR_INVALID_ARGUMENT;
	const vpn_t vpn = ((uintptr_t)failed_address & (~((1 << (12 + 9 * as.page_size)) - 1) & 0xFFFFFFFFFFFFFFFF)) >> 12;
	ppn_t ppn = 0;
	const error_t alloc_err = allocate_page_frame(as.page_size, &ppn);
	// TODO: handle alloc_err
	const error_t ptae_err = page_table_add_entry(as.page_table, as.page_size, vpn, ppn, R, W, X);
	if(ptae_err) return ERR_CRITICAL_INTERNAL_FAILURE;
	return ERR_NONE;
}
