#include "bootstrap_page_table.h"

#include <stdbigos/csr.h>
#include <stdbigos/string.h>

//===============================
//===        INTERNAL        ====
//===============================

static void* s_page_frame_allocation_region = nullptr;
static u64 s_amount_of_4kB_pages = 0;
static u64 s_amount_of_2MB_pages = 0;
static void* s_ram_start = nullptr;
static virtual_memory_scheme_t s_active_vms = VMS_BARE;

reg_t create_satp(virtual_memory_scheme_t vms, u16 asid, ppn_t ppn) {
	return ((u64)((vms == -1 ? 0 : vms + 8)) << 60) | ((u64)asid << 44) | ((u64)ppn & 0xfffffffffff);
}

void read_satp(reg_t satp, virtual_memory_scheme_t* vmsOUT, u16* asidOUT, ppn_t* ppnOUT) {
	if(vmsOUT) {
		virtual_memory_scheme_t mode = satp >> 60;
		*vmsOUT = (mode == 0) ? -1 : mode - 8;
	}
	if(asidOUT) *asidOUT = (satp >> 44) & UINT16_MAX;
	if(ppnOUT) *ppnOUT = satp & 0xfffffffffff;
}

u64 create_page_table_entry(u8 flags, u8 rsw, ppn_t ppn) {
	return ((u64)flags & 0xff) | (((u64)rsw << 8) & 0x3) | (((u64)ppn << 10) & 0xfffffffffff);
}

void read_page_table_entry(u64 pte, ppn_t* ppnOUT) {
	if(ppnOUT) *ppnOUT = (pte >> 10) & 0xfffffffffff;
}

bool add_to_vpn_reg(u64 vpn_reg[], u8* inx, u64 val) {
	for(u8 i = 0; i < *inx; ++i) {
		if(vpn_reg[i] == val) return true;
	}
	vpn_reg[*inx++] = val;
	if(*inx > 63) return false;
	return true;
}

ppn_t get_page_frame(page_size_t ps) {
	static bool not_init = true;
	static void* address_of_4k = nullptr;
	static void* address_of_2M = nullptr;
	if(not_init) {
		not_init = false;
		address_of_4k = s_page_frame_allocation_region;
		address_of_2M = s_page_frame_allocation_region + 0x200000 * ((s_amount_of_4kB_pages + 511) / 512);
	}
	switch(ps) {
	case PAGE_SIZE_4kB: {
		u64 address = (uintptr_t)address_of_4k;
		address_of_4k += 0x1000;
		return address >> 12;
	}
	case PAGE_SIZE_2MB: {
		u64 address = (uintptr_t)address_of_2M;
		address_of_4k += 0x200000;
		return address >> 12;
	}
	default: return 0; // PANIC
	}
}

void page_table_add_entry(u64 root_pt_ppn, page_size_t ps, vpn_t vpn, ppn_t ppn, bool R, bool W, bool X) {
	const u16 vpn_slice[5] = {
		(vpn >> (9 * 0)) & 0x1ffu, (vpn >> (9 * 1)) & 0x1ffu, (vpn >> (9 * 2)) & 0x1ffu,
		(vpn >> (9 * 3)) & 0x1ffu, (vpn >> (9 * 4)) & 0x1ffu,
	};
	u64* curr_pte = s_ram_start + (root_pt_ppn << 12);
	for(i8 level = s_active_vms + 2; level >= ps; --level) {
		ppn_t curr_ppn = 0;
		read_page_table_entry(*curr_pte, &curr_ppn);
		curr_pte = &(*(u64(*)[512])(s_ram_start + (curr_ppn << 12)))[vpn_slice[level]];
		ppn_t new_ppn = 0;
		if(level == ps)
			new_ppn = ppn;
		else {
			new_ppn = get_page_frame(PAGE_SIZE_4kB);
			memset((new_ppn << 12) + s_ram_start, 0, 0x1000);
		}
		u8 access_perms = 0;
		if(level == ps) {
			if(R) access_perms |= (1 << 1);
			if(W) access_perms |= (1 << 2);
			if(X) access_perms |= (1 << 3);
		}
		*curr_pte = create_page_table_entry((1 << 0) | (1 << 5) | (1 << 4) | access_perms, 0, new_ppn);
	}
}

//===============================
//===    !!! INTERNAL !!!    ====
//===============================

void init_boot_page_table_managment(virtual_memory_scheme_t vms, void* ram_start) {
	s_active_vms = vms;
	s_ram_start = ram_start;
}

required_memory_space_t calc_required_memory_for_page_table(region_t regions[], u64 regions_amount) {
	u64 vpn_reg[64] = {0};
	u8 vpn_reg_inx = 0;

	u64 page_2MB = 0;
	u64 page_4kB = 0;

	for(u64 i = 0; i < regions_amount; ++i) {
		u64 size_left = regions[i].size;
		u64 working_address = regions[i].addr;
		while(size_left > 0) {
			if(!add_to_vpn_reg(vpn_reg, &vpn_reg_inx, (working_address >> (9 + 12))))
				return (required_memory_space_t){0, 0, 0, .error = true};
			if(!regions[i].mapped) ++page_2MB;
			working_address += 0x200000;
			if(size_left < 0x200000)
				size_left = 0;
			else
				size_left -= 0x200000;
		}
	}
	page_4kB += vpn_reg_inx - 1;
	for(u8 lvl = 2; lvl < s_active_vms + 3; ++lvl) {
		u64 new_vpn_reg[64] = {0};
		u8 new_vpn_reg_inx = 0;
		for(u8 i = 0; i < vpn_reg_inx; ++i)
			if(!add_to_vpn_reg(new_vpn_reg, &new_vpn_reg_inx, vpn_reg[i] >> 9))
				return (required_memory_space_t){0, 0, 0, .error = true};
		page_4kB += new_vpn_reg_inx;
		vpn_reg_inx = new_vpn_reg_inx;
		memcpy(vpn_reg, new_vpn_reg, new_vpn_reg_inx * sizeof(u64));
	}
	return (required_memory_space_t){.amount_of_4kb_pages = page_4kB,
									 .amount_of_2Mb_pages = page_2MB,
									 .total_in_bytes = page_4kB * 0x1000 + page_2MB * 0x200000,
									 .error = false};
}

u16 initialize_virtual_memory() {
	reg_t satp = create_satp(VMS_BARE, UINT16_MAX, 0);
	CSR_WRITE(satp, satp);
	satp = CSR_READ(satp);
	u16 asid_max_val = 0;
	read_satp(satp, nullptr, &asid_max_val, nullptr);
	return asid_max_val;
}

page_table_meta_t create_page_table(region_t regions[], u64 regions_amount, void* mem_region) {
	s_page_frame_allocation_region = mem_region;
	ppn_t root_ppn = get_page_frame(PAGE_SIZE_4kB);
	memset((root_ppn << 12) + s_ram_start, 0, 0x1000);
	for(u64 i = 0; i < regions_amount; ++i) {
		u64 size_left = regions[i].size;
		u64 curr_addr = regions[i].addr;
		u64 map_addr = regions[i].map_address;
		while(size_left > 0) {
			ppn_t use_ppn = (regions[i].mapped) ? (map_addr << 12) : get_page_frame(PAGE_SIZE_2MB);
			page_table_add_entry(root_ppn, PAGE_SIZE_2MB, curr_addr >> 12, use_ppn, 1, 1, 1);
			if(regions[i].mapped) map_addr += 0x200000;
			curr_addr += 0x200000;
			size_left = (size_left < 0x200000) ? 0 : size_left - 0x200000;
		}
	}
	return (page_table_meta_t){
		.root_pt_ppn = root_ppn, .used_4kB_pages = s_amount_of_4kB_pages, .used_2MB_pages = s_amount_of_2MB_pages};
}
