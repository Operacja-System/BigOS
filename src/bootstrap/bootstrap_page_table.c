#include "bootstrap_page_table.h"

#include <debug/debug_stdio.h>
#include <stdbigos/csr.h>
#include <stdbigos/string.h>

#include "bootstrap_memory_services.h"

//===============================
//===        INTERNAL        ====
//===============================

static virtual_memory_scheme_t s_active_vms = VMS_BARE;
static physical_memory_region_t s_page_mem_regs[5] = {0};

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

ppn_t get_page_frame(page_size_t ps) {
	static u64 number_of_allocated_pages[PAGE_SIZE_AMOUNT] = {0};
	const u64 region_offset = (number_of_allocated_pages[ps]++) * ((4 * kiB) << (9 * ps));
	return (u64)(s_page_mem_regs[ps].address + region_offset) >> 12;
}

static void page_table_add_entry(u64 root_pt_ppn, page_size_t ps, vpn_t vpn, ppn_t ppn, bool R, bool W, bool X) {
	u16 vpn_slice[5] = {
		(vpn >> 9 * 0) & 0x1ff, (vpn >> 9 * 1) & 0x1ff, (vpn >> 9 * 2) & 0x1ff,
		(vpn >> 9 * 3) & 0x1ff, (vpn >> 9 * 4) & 0x1ff,
	};
	u64(*current_page)[512] = (u64(*)[512])(root_pt_ppn << 12);
	for(i8 lvl = s_active_vms + 2; lvl > ps; --lvl) {
		u8 flags = 0;
		ppn_t current_ppn = 0;
		u64* current_pte = &(*current_page)[vpn_slice[lvl]];
		read_page_table_entry(*current_pte, &flags, nullptr, &current_ppn);
		if((flags & PTEF_V) == 0) {
			current_ppn = get_page_frame(PAGE_SIZE_4kB);
			memset((void*)(current_ppn << 12), 0, 4 * kiB);
			*current_pte = create_page_table_entry(PTEF_V | PTEF_G, 0, current_ppn);
		}
		current_page = (u64(*)[512])(current_ppn << 12);
	}
	u8 flags = PTEF_V | PTEF_G;
	if(R) flags |= PTEF_R;
	if(W) flags |= PTEF_W;
	if(X) flags |= PTEF_X;
	(*current_page)[vpn_slice[ps]] = create_page_table_entry(flags, 0, ppn);
}

static bool add_to_vpn_reg(u64 vpn_reg[], u32* inx, u64 val, u64 size) {
	for(u32 i = 0; i < *inx; ++i) {
		if(vpn_reg[i] == val) return true;
	}
	vpn_reg[(*inx)++] = val;
	return *inx < size;
}

//===============================
//===      ! INTERNAL        ====
//===============================

u16 initialize_virtual_memory(virtual_memory_scheme_t vms) {
	s_active_vms = vms;
	reg_t satp = create_satp(VMS_BARE, UINT16_MAX, 0);
	CSR_WRITE(satp, satp);
	satp = CSR_READ(satp);
	u16 asid_max_val = 0;
	read_satp(satp, nullptr, &asid_max_val, nullptr);
	return asid_max_val;
}

required_memory_space_t calc_required_memory_for_page_table(region_t* regions, u64 regions_amount) {
	constexpr u32 vpn_reg_size = 256;
	u64 vpn_reg[vpn_reg_size] = {0};
	u32 vpn_reg_inx = 0;
	u32 max_vpn_reg_inx = 0;

	u64 page_amounts[PAGE_SIZE_AMOUNT] = {0};

	for(u8 lvl = 0; lvl <= s_active_vms + 3; ++lvl) {
		if(vpn_reg_inx > max_vpn_reg_inx) max_vpn_reg_inx = vpn_reg_inx;
		u64 new_vpn_reg[vpn_reg_size] = {0};
		u32 new_vpn_reg_inx = 0;
		for(u32 i = 0; i < vpn_reg_inx; ++i) {
			if(!add_to_vpn_reg(new_vpn_reg, &new_vpn_reg_inx, vpn_reg[i] >> 9, vpn_reg_size))
				return (required_memory_space_t){{0}, .error = ERR_CRITICAL_INTERNAL_FAILURE};
		}
		page_amounts[PAGE_SIZE_4kB] += new_vpn_reg_inx;
		for(u64 i = 0; i < regions_amount; ++i) {
			if(regions[i].ps == lvl) {
				u64 size_left = regions[i].size;
				u64 curr_addr = regions[i].addr;
				const u64 size_dif = (4 * kiB) << (9 * lvl);
				while(size_left > 0) {
					u32 old_inx = new_vpn_reg_inx;
					if(!add_to_vpn_reg(new_vpn_reg, &new_vpn_reg_inx, curr_addr >> (12 + 9 * lvl), vpn_reg_size))
						return (required_memory_space_t){{0}, .error = ERR_CRITICAL_INTERNAL_FAILURE};
					if(old_inx == new_vpn_reg_inx) {
						DEBUG_PRINTF("overaping mem regions, i: %lu page: %lu\n", i,
									 (curr_addr - regions[i].addr) / size_dif);
					}
					if(!regions[i].mapped) ++page_amounts[lvl];
					curr_addr += size_dif;
					if(size_left < size_dif) size_left = size_dif;
					size_left -= size_dif;
				}
			}
		}
		vpn_reg_inx = new_vpn_reg_inx;
		memcpy(vpn_reg, new_vpn_reg, new_vpn_reg_inx * sizeof(u64));
	}
	required_memory_space_t ret = {{0}, ERR_NONE};
	memcpy(ret.require_page_amounts, page_amounts, sizeof(page_amounts));
	return ret;
}

void set_page_memory_regions(physical_memory_region_t mem_regions[5]) {
	memcpy(s_page_mem_regs, mem_regions, 5 * sizeof(physical_memory_region_t));
	const char* page_names[] = {"kilo", "mega", "giga", "tera", "peta"};
	DEBUG_PRINTF("[i] page memory region set:\n");
	for(u8 i = 0; i < 5; ++i) {
		DEBUG_PRINTF("\t[i] %s page to: 0x%lx, size: 0x%lx\n", page_names[i], (u64)mem_regions[i].address,
					 mem_regions[i].size);
	}
}

ppn_t create_page_table(region_t regions[], u64 regions_amount) {
	ppn_t root_ppn = get_page_frame(PAGE_SIZE_4kB);
	memset((void*)(root_ppn << 12), 0, (4 * kiB));
	for(u64 i = 0; i < regions_amount; ++i) {
		u64 size_left = regions[i].size;
		u64 curr_addr = regions[i].addr;
		u64 map_addr = regions[i].map_address;
		const u64 page_size = (4 * kiB) << (9 * regions[i].ps);
		while(size_left > 0) {
			ppn_t use_ppn = (regions[i].mapped) ? (map_addr >> 12) : get_page_frame(regions[i].ps);
			page_table_add_entry(root_ppn, regions[i].ps, curr_addr >> 12, use_ppn, true, true, true);
			if(regions[i].mapped) map_addr += page_size;
			curr_addr += page_size;
			if(size_left < page_size) size_left = page_size;
			size_left -= page_size;
		}
	}
	return root_ppn;
}

void print_page_table(ppn_t root_ppn, u8 start_lvl, vpn_t vpn) {
	u64(*page_table)[512] = (u64(*)[512])(root_ppn << 12);
	for(u16 i = 0; i < 512; ++i) {
		ppn_t pte_ppn = 0;
		u8 pte_rsw = 0;
		u8 pte_flags = 0;
		read_page_table_entry((*page_table)[i], &pte_flags, &pte_rsw, &pte_ppn);
		if(!(pte_flags & PTEF_V)) continue;
		vpn_t new_vpn = (vpn << 9) | i;
		if((pte_flags & PTEF_X) || (pte_flags & PTEF_W) || (pte_flags & PTEF_R)) {
			u64 va = (new_vpn << 9 * (s_active_vms + 2 - start_lvl)) << 12;
			char rsw_str[2 + 1] = {0} ;
			rsw_str[0] = (pte_rsw & (1 << 0)) ? 'X' : '-';
			rsw_str[1] = (pte_rsw & (1 << 1)) ? 'X' : '-';
			char flags_str[8 + 1] = {0} ;
			flags_str[0] = (pte_flags & PTEF_D) ? 'D' : '-';
			flags_str[1] = (pte_flags & PTEF_A) ? 'A' : '-';
			flags_str[2] = (pte_flags & PTEF_G) ? 'G' : '-';
			flags_str[3] = (pte_flags & PTEF_U) ? 'U' : '-';
			flags_str[4] = (pte_flags & PTEF_X) ? 'X' : '-';
			flags_str[5] = (pte_flags & PTEF_W) ? 'W' : '-';
			flags_str[6] = (pte_flags & PTEF_R) ? 'R' : '-';
			flags_str[7] = (pte_flags & PTEF_V) ? 'V' : '-';
			const u64 page_size = (4 * kiB) << 9 * (s_active_vms + 2 - start_lvl);
			char* ps_str = {0};
			if(page_size == (4 * kiB)) ps_str = " kilo ";
			else if(page_size == (2 * MiB)) ps_str = " mega ";
			else if(page_size == GiB) ps_str = " giga ";
			else ps_str = "broken";
			DEBUG_PRINTF("%016lx - %016lx | page size: %s | ppn: 0x%lx | rsw: %s | flags: %s\n" ,
				va, va + page_size, ps_str, pte_ppn, rsw_str, flags_str);
		}
		else print_page_table(pte_ppn, start_lvl + 1, new_vpn);
	}
}
