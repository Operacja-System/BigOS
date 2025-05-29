#include "bootstrap_page_table.h"

#include <debug/debug_stdio.h>
#include <stdbigos/csr.h>
#include <stdbigos/string.h>

#include "bootstrap_memory_services.h"
#include "bootstrap_panic.h"

//===============================
//===        INTERNAL        ====
//===============================

static virtual_memory_scheme_t s_active_vms = VMS_BARE;
static phisical_memory_region_t s_page_mem_regs[5] = {0};

static inline reg_t create_satp(virtual_memory_scheme_t vms, u16 asid, ppn_t ppn) {
	return ((u64)((vms == -1 ? 0 : vms + 8)) << 60) | ((u64)asid << 44) | ((u64)ppn & 0xfffffffffff);
}

static void read_satp(reg_t satp, virtual_memory_scheme_t* vmsOUT, u16* asidOUT, ppn_t* ppnOUT) {
	if(vmsOUT) {
		virtual_memory_scheme_t mode = satp >> 60;
		*vmsOUT = (mode == 0) ? -1 : mode - 8;
	}
	if(asidOUT) *asidOUT = (satp >> 44) & UINT16_MAX;
	if(ppnOUT) *ppnOUT = satp & 0xfffffffffff;
}

static inline u64 create_page_table_entry(u8 flags, u8 rsw, ppn_t ppn) {
	return ((u64)flags & 0xff) | ((u64)(rsw & 0b11) << 8) | ((u64)(ppn & 0xfffffffffff) << 10);
}

static inline void read_page_table_entry(u64 pte, u8* flagsOUT, u8* rswOUT, ppn_t* ppnOUT) {
	if(flagsOUT) *flagsOUT = (pte >> 0) & 0xff;
	if(rswOUT) *rswOUT = (pte >> 8) & 0b11;
	if(ppnOUT) *ppnOUT = (pte >> 10) & 0xfffffffffff;
}

static bool add_to_vpn_reg(u64 vpn_reg[], u32* inx, u64 val, u64 size) {
	for(u32 i = 0; i < *inx; ++i) {
		if(vpn_reg[i] == val) return true;
	}
	vpn_reg[(*inx)++] = val;
	if(*inx >= size) return false;
	return true;
}

ppn_t get_page_frame(page_size_t ps) {
	static u64 number_of_allocated_pages[PAGE_SIZE_AMOUNT] = {0};
	return (u64)(s_page_mem_regs[ps].address + number_of_allocated_pages[ps] * (0x1000 << (9 * ps))) >> 12;
}

static void page_table_add_entry(u64 root_pt_ppn, page_size_t ps, vpn_t vpn, ppn_t ppn, bool R, bool W, bool X) {
	vpn >>= 12;
	const u16 vpn_slice[5] = {
		(vpn >> (9 * 0)) & 0x1ffu, (vpn >> (9 * 1)) & 0x1ffu, (vpn >> (9 * 2)) & 0x1ffu,
		(vpn >> (9 * 3)) & 0x1ffu, (vpn >> (9 * 4)) & 0x1ffu,
	};
	u64* curr_pte = (void*)(root_pt_ppn << 12);
	for(i8 level = s_active_vms + 2; level >= ps; --level) {
		u8 flags;
		ppn_t curr_ppn = 0;
		read_page_table_entry(*curr_pte, &flags, nullptr, &curr_ppn);
		curr_pte = &(*(u64(*)[512])((curr_ppn << 12)))[vpn_slice[level]];
		ppn_t new_ppn = 0;
		if(level == ps)
			new_ppn = ppn;
		else if (flags & PTEF_V) {
			curr_pte = (void*)(curr_ppn << 12);
			continue;
		}
		else {
			new_ppn = get_page_frame(PAGE_SIZE_4kB);
			memset((void*)(new_ppn << 12), 0, 0x1000);
		}
		u8 access_perms = 0;
		if(level == ps) {
			if(R) access_perms |= PTEF_R;
			if(W) access_perms |= PTEF_W;
			if(X) access_perms |= PTEF_X;
		}
		*curr_pte = create_page_table_entry(PTEF_V | PTEF_G | access_perms, 0, new_ppn);
	}
}

//===============================
//===    !!! INTERNAL !!!    ====
//===============================

void init_boot_page_table_managment(virtual_memory_scheme_t vms) {
	s_active_vms = vms;
}

void set_page_memory_regions(phisical_memory_region_t mem_regions[5]) {
	memcpy(s_page_mem_regs, mem_regions, 5 * sizeof(phisical_memory_region_t));
	const char* page_names[] = {"kilo", "mega", "giga", "tera", "peta"};
	DEBUG_PRINTF("[ ] page memory region set:\n");
	for(u8 i = 0; i < 5; ++i) {
		DEBUG_PRINTF("\t[ ] %s page to: %lx, size: %lu\n", page_names[i], (u64)mem_regions[i].address,
					 mem_regions[i].size);
	}
}

required_memory_space_t calc_required_memory_for_page_table(region_t* regions, u64 regions_amount) {
	constexpr u32 vpn_reg_size = 1024;
	u64 vpn_reg[vpn_reg_size] = {0};
	u32 vpn_reg_inx = 0;
	u32 max_vpn_reg_inx = 0;

	u64 page_amounts[PAGE_SIZE_AMOUNT] = {0};

	for(u8 lvl = 0; lvl <= s_active_vms + 3; ++lvl) {
		if(vpn_reg_inx > max_vpn_reg_inx) max_vpn_reg_inx = vpn_reg_inx;
		u64 new_vpn_reg[vpn_reg_size] = {0};
		u32 new_vpn_reg_inx = 0;
		for(u32 i = 0; i < vpn_reg_inx; ++i)
			if(!add_to_vpn_reg(new_vpn_reg, &new_vpn_reg_inx, vpn_reg[i] >> 9, vpn_reg_size))
				return (required_memory_space_t){{0}, .error = true};
		page_amounts[PAGE_SIZE_4kB] += new_vpn_reg_inx;
		for(u64 i = 0; i < regions_amount; ++i) {
			if(regions[i].ps == lvl) {
				u64 size_left = regions[i].size;
				u64 curr_addr = regions[i].addr;
				const u64 size_dif = 0x1000 << (9 * lvl);
				while(size_left > 0) {
					u32 old_inx = new_vpn_reg_inx;
					if(!add_to_vpn_reg(new_vpn_reg, &new_vpn_reg_inx, curr_addr >> (12 + 9 * lvl), vpn_reg_size))
						return (required_memory_space_t){{0}, .error = true};
					if(old_inx == new_vpn_reg_inx) {
						DEBUG_PRINTF("overaping mem regions, i: %lu page: %lu\n", i,
									 (curr_addr - regions[i].addr) / size_dif);
					}
					if(regions[i].mapped == false) ++page_amounts[lvl];
					curr_addr += size_dif;
					if(size_left < size_dif) size_left = size_dif;
					size_left -= size_dif;
				}
			}
		}
		vpn_reg_inx = new_vpn_reg_inx;
		memcpy(vpn_reg, new_vpn_reg, new_vpn_reg_inx * sizeof(u64));
	}
	required_memory_space_t ret = {{0}, false};
	memcpy(ret.require_page_amounts, page_amounts, sizeof(page_amounts));
	return ret;
}

u16 initialize_virtual_memory() {
	reg_t satp = create_satp(VMS_BARE, UINT16_MAX, 0);
	CSR_WRITE(satp, satp);
	satp = CSR_READ(satp);
	u16 asid_max_val = 0;
	read_satp(satp, nullptr, &asid_max_val, nullptr);
	return asid_max_val;
}

ppn_t create_page_table(region_t regions[], u64 regions_amount) {
	ppn_t root_ppn = get_page_frame(PAGE_SIZE_4kB);
	memset((void*)(root_ppn << 12), 0, 0x1000);
	for(u64 i = 0; i < regions_amount; ++i) {
		u64 size_left = regions[i].size;
		u64 curr_addr = regions[i].addr;
		u64 map_addr = regions[i].map_address;
		while(size_left > 0) {
			ppn_t use_ppn = (regions[i].mapped) ? (map_addr >> 12) : get_page_frame(regions[i].ps);
			page_table_add_entry(root_ppn, regions[i].ps, curr_addr >> 12, use_ppn, 1, 1, 1);
			const u64 size_dif = 0x1000 << (9 * regions[i].ps);
			if(regions[i].mapped) map_addr += size_dif;
			curr_addr += size_dif;
			if(size_left < size_dif) size_left = size_dif;
			size_left -= size_dif;
		}
	}
	return root_ppn;
}
