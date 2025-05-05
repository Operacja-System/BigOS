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
int first_zero_bit_index(u64 x) {
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

typedef enum : u8 {
	SATP_MODE_BARE = 0,
	SATP_MODE_SV39 = 8,
	SATP_MODE_SV48 = 9,
	SATP_MODE_SV57 = 10,
	SATP_MODE_SV64 = 11,
} satp_mode_t;

reg_t create_satp(satp_mode_t mode, u16 asid, ppn_t ppn) {
	return ((u64)mode << 60) | ((u64)asid << 44) | ((u64)ppn & 0xfffffffffff);
}

void read_satp(reg_t satp, satp_mode_t* modeOUT, u16* asidOUT, ppn_t* ppnOUT) {
	if(modeOUT) *modeOUT = satp >> 60;
	if(asidOUT) *asidOUT = (satp >> 44) & UINT16_MAX;
	if(ppnOUT) *ppnOUT = satp & 0xfffffffffff;
}

error_t initialize_virtual_memory(virtual_memory_scheme_t vms) {
	reg_t satp = create_satp(SATP_MODE_BARE, UINT16_MAX, 0);
	CSR_WRITE(satp, satp);
	satp = CSR_READ(satp);
	u16 asid_max_val = 0;
	read_satp(satp, nullptr, &asid_max_val, nullptr);
	if(asid_max_val == 0) return ERR_HARDWARE_NOT_COMPATIBLE;
	asidlen = count_bits_u64(asid_max_val);
	active_vms = vms;
	return ERR_NONE;
}

error_t enable_virtual_memory(asid_t asid) {
	if(active_vms == VMS_BARE) return ERR_CRITICAL_INTERNAL_FAILURE;
	ppn_t ppn = 0;
	read_page_table_entry(address_spaces[asid].page_table, nullptr, nullptr, &ppn);
	reg_t satp = create_satp((satp_mode_t[3]){SATP_MODE_SV39, SATP_MODE_SV48, SATP_MODE_SV57}[active_vms], asid, ppn);
	CSR_WRITE(satp, satp);
	return ERR_NONE;
}

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
