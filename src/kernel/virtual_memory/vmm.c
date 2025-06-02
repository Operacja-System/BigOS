#include "vmm.h"

#include <stdbigos/csr.h>

#include "address_space.h"
#include "virtual_memory/page_table.h"

static virtual_memory_scheme_t active_vms = VMS_BARE;
static u8 asidlen = 0;

static constexpr size_t address_spaces_size = UINT16_MAX;
static constexpr size_t address_spaces_valid_bit_size = UINT16_MAX / sizeof(u64);
static address_space_t address_spaces[address_spaces_size] = {0};
static u64 address_spaces_valid_bit[address_spaces_valid_bit_size] = {0};

// TODO: replace those functons with stdbit.h
static int first_zero_bit_index(u64 x) {
	for(int i = 0; i < 64; ++i) {
		if(((x >> i) & 1) == 0) return i;
	}
	return -1;
}

u8 count_bits_u64(u64 x) {
	x = x - ((x >> 1) & 0x5555555555555555ULL);
	x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
	x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
	x = x + (x >> 8);
	x = x + (x >> 16);
	x = x + (x >> 32);
	return x & 0x7F;
}
// ENDTODO

error_t create_address_space(page_size_t ps, bool global, bool user, asid_t* asidOUT) {
	bool found = false;
	asid_t new_asid = 0;
	for(u64 i = 0; i < address_spaces_valid_bit_size; ++i) {
		int inx = first_zero_bit_index(address_spaces_valid_bit[i]);
		if(inx == -1) continue;
		address_spaces_valid_bit[i] |= (1 << inx);
		new_asid = i * 64 + inx;
		found = true;
	}
	if(!found) return ERR_ALL_ADDRESS_SPACES_IN_USE;
	const error_t err = address_space_create(ps, global, user, &address_spaces[new_asid]);
	if(err) return err;
	*asidOUT = new_asid;
	return ERR_NONE;
}

error_t destroy_address_space(asid_t asid) {
	const error_t err = address_space_destroy(&address_spaces[asid]);
	if(err) return ERR_INVALID_ARGUMENT;
	address_spaces_valid_bit[asid / 64] &= ~(1 << (asid % 64));
	return ERR_NONE;
}

error_t resolve_page_fault(asid_t asid, void* failed_address) {
	// TODO: this permmisions need to be handled properly
	const error_t err = address_space_handle_page_fault(address_spaces[asid], failed_address, true, true, true);
	if(err) return err;
	return ERR_NONE;
}

virtual_memory_scheme_t get_active_virtual_memory_scheme() {
	return active_vms;
}
