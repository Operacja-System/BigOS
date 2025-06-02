#include "bootstrap_memory_services.h"

#include <debug/debug_stdio.h>
#include <stdbigos/string.h>

// TODO: This shouldn't be here
u64 align_up(u64 val, u64 align) {
	return (val + align - 1) & ~(align - 1);
}

static void* s_mem_start = nullptr;
static u64 s_mem_size = 0;

void set_ram_params(void* ram_start, u64 ram_size) {
	s_mem_start = ram_start;
	s_mem_size = ram_size;
}

void* get_ram_start() {
	return s_mem_start;
}

u64 get_ram_size() {
	return s_mem_size;
}

// TODO: This also shouldn't be here
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

// I know this algorithm is very naive, but busy memory regions amount most likely won't exceed 6
error_t allocate_physical_memory_region(void* dt, u64 size, u64 align, physical_memory_region_t* pmrOUT) {
	static physical_memory_region_t allocated_memory_regions[16] = {0};
	static u64 allocated_regions_end_idx = 0;
	if(allocated_regions_end_idx >= sizeof(allocated_memory_regions) / sizeof(allocated_memory_regions[0]))
		return ERR_CRITICAL_INTERNAL_FAILURE;
	//TEST:
	u64 unavaliable_regions_amount = 0;
	unavaliable_regions_amount = 1;
	physical_memory_region_t unavaliable_regions[unavaliable_regions_amount];
	unavaliable_regions[0] = (physical_memory_region_t){.address = (void*)0x80000000, .size = 160 * kiB};
	// TODO: read the rest of the unavalible memory regions and the region of kboot from the dt
	//!TEST
	void* curr_region_start = (void*)align_up((u64)get_ram_start(), align);

	physical_memory_region_t* busy_regions_ptrs[] = {allocated_memory_regions, unavaliable_regions};
	u64 busy_regions_sizes[] = {allocated_regions_end_idx, unavaliable_regions_amount};
	while((curr_region_start + size) < (get_ram_start() + get_ram_size())) {
		bool overlap = false;
		for(u64 reg_idx = 0; reg_idx < sizeof(busy_regions_ptrs) / sizeof(busy_regions_ptrs[0]); ++reg_idx) {
			for(u64 i = 0; i < busy_regions_sizes[reg_idx]; ++i) {
				void* busy_region_start = busy_regions_ptrs[reg_idx][i].address;
				void* busy_region_end = busy_regions_ptrs[reg_idx][i].address + busy_regions_ptrs[reg_idx][i].size;
				if(MAX(curr_region_start, busy_region_start) < MIN(curr_region_start + size, busy_region_end)) {
					curr_region_start = busy_region_end;
					curr_region_start = (void*)align_up((u64)curr_region_start, align);
					overlap = true;
					break;
				}
			}
			if(overlap) break;
		}
		if(!overlap) {
			*pmrOUT = (physical_memory_region_t){.address = curr_region_start, .size = size};
			allocated_memory_regions[allocated_regions_end_idx++] = *pmrOUT;
			return ERR_NONE;
		}
	}
	return ERR_PHYSICAL_MEMORY_FULL;
}

typedef enum : u8 {
	OSABI_SYSTEM_V = 0x00,
	OSABI_HP_UX = 0x01,
	OSABI_NETBSD = 0x02,
	OSABI_LINUX = 0x03,
	OSABI_GNU_HURD = 0x04,
	OSABI_SOLARIS = 0x06,
	OSABI_AIX = 0x07,
	OSABI_IRIX = 0x08,
	OSABI_FREEBSD = 0x09,
	OSABI_TRU64 = 0x0a,
	OSABI_NOVELL_MODESTO = 0x0b,
	OSABI_OPENBSD = 0x0c,
	OSABI_OPENVMS = 0x0d,
	OSABI_NONSTOP_KERNEL = 0x0e,
	OSABI_AROS = 0x0f,
	OSABI_FENIXOS = 0x10,
	OSABI_NUXI_CLOUDABI = 0x11,
	OSABI_OPENVOS = 0x12,
} os_abi_t;

typedef enum : u16 {
	ET_NONE = 0x00,
	ET_REL = 0x01,
	ET_EXEC = 0x02,
	ET_DYN = 0x03,
	ET_CORE = 0x04,
} e_type_t;

typedef enum : u16 { // NOTE: Documentation provides more defines but we will never support other architectures
	EM_NONE = 0x00,
	EM_X86 = 0x03,
	EM_ARM = 0x28,
	EM_AMD_X86_64 = 0x3e,
	EM_ARM_64 = 0xb7,
	EM_RISCV = 0xf3,
} e_machine_t;

typedef struct [[gnu::packed]] {
	char magic[4];
	u8 class;
	u8 data;
	u8 version_ident;
	os_abi_t OS_ABI;
	u8 ABI_version;
	u8 padding[7];
	e_type_t type;
	e_machine_t machine;
	u32 version;
	u64 entry;
	u64 start_of_program_headers;
	u64 start_of_section_headers;
	u32 flags;
	u16 size_of_this_header;
	u16 size_of_program_headrs;
	u16 number_of_program_headers;
	u16 size_of_section_headers;
	u16 number_of_sections_headers;
	u16 section_header_string_table_index;
} elf64_header_t;

typedef enum : u32 {
	PT_NULL = 0x00000000,
	PT_LOAD = 0x00000001,
	PT_DYNAMIC = 0x00000002,
	PT_INTERP = 0x00000003,
	PT_NOTE = 0x00000004,
	PT_SHLIB = 0x00000005,
	PT_PHDR = 0x00000006,
	PT_TLS = 0x00000007,
} p_type_t;

typedef enum : u32 {
	PF_X = 0x1,
	PF_W = 0x2,
	PF_R = 0x4,
} p_flags_t;

typedef struct [[gnu::packed]] {
	p_type_t type;
	p_flags_t flags;
	u64 offset;
	u64 vaddr;
	u64 paddr;
	u64 filesz;
	u64 memsz;
	u8 align;
} elf64_program_header_t;

typedef enum : u32 {
	SHT_NULL = 0x0,
	SHT_PROGBITS = 0x1,
	SHT_SYMTAB = 0x2,
	SHT_STRTAB = 0x3,
	SHT_RELA = 0x4,
	SHT_HASH = 0x5,
	SHT_DYNAMIC = 0x6,
	SHT_NOTE = 0x7,
	SHT_NOBITS = 0x8,
	SHT_REL = 0x9,
	SHT_SHLIB = 0x0A,
	SHT_DYNSYM = 0x0B,
	SHT_INIT_ARRAY = 0x0E,
	SHT_FINI_ARRAY = 0x0F,
	SHT_PREINIT_ARRAY = 0x10,
	SHT_GROUP = 0x11,
	SHT_SYMTAB_SHNDX = 0x12,
	SHT_NUM = 0x13,
} sh_type_t;

typedef enum : u64 {
	SHF_WRITE = 0x1,
	SHF_ALLOC = 0x2,
	SHF_EXECINSTR = 0x4,
	SHF_MERGE = 0x10,
	SHF_STRINGS = 0x20,
	SHF_INFO_LINK = 0x40,
	SHF_LINK_ORDER = 0x80,
	SHF_OS_NONCONFORMING = 0x100,
	SHF_GROUP = 0x200,
	SHF_TLS = 0x400,
	SHF_MASKOS = 0x0FF00000,
	SHF_MASKPROC = 0xF0000000,
	SHF_ORDERED = 0x4000000,
	SHF_EXCLUDE = 0x8000000,
} sh_flags_t;

typedef struct [[gnu::packed]] {
	u32 name_off;
	sh_type_t type;
	sh_flags_t flags;
	u64 addr;
	u64 offset;
	u64 size;
	u32 link;
	u32 info;
	u64 addr_align;
	u64 entry_size;
} elf64_section_header_t;

static void load_elf_segment_at_address(void* elf_img, elf64_program_header_t ph, void* target_addr) {
	const void* segment_start = elf_img + ph.offset;
	void* load_addr = target_addr + align_up(ph.vaddr, ph.align);
	memcpy(load_addr, segment_start, ph.filesz);
	memset(load_addr + ph.filesz, 0, ph.memsz - ph.filesz);
}

error_t load_elf_at_address(void* elf_img, void* target_addr, void** elf_entry_OUT) {
	const elf64_header_t* header = elf_img;
	if(((u64)target_addr & 0xfff) != 0) // if targed_addr is not aligned to 4kiB then elf alignments will be broken
		return ERR_INVALID_ARGUMENT;
	if(header->magic[0] != 0x7f || header->magic[1] != 'E' || header->magic[2] != 'L' || header->magic[3] != 'F')
		return ERR_INVALID_ARGUMENT;
	if(header->version != 0x01) return ERR_INVALID_ARGUMENT;
	if(header->class != 0x02)		// Only elf64 is supported
		return ERR_INVALID_ARGUMENT;
	if(header->data != 0x01)		// Only little-endian is supported
		return ERR_INVALID_ARGUMENT;
	if(header->machine != EM_RISCV) // Only riscv is supported
		return ERR_INVALID_ARGUMENT;
	if(header->entry == 0)			// ELF doesnt have an entry point
		return ERR_INVALID_ARGUMENT;
	*elf_entry_OUT = target_addr + header->entry;
	const elf64_program_header_t* segment_table = elf_img + header->start_of_program_headers;
	for(u64 i = 0; i < header->size_of_program_headrs; ++i) {
		if(segment_table[i].type == PT_LOAD) load_elf_segment_at_address(elf_img, segment_table[i], target_addr);
	}
	return ERR_NONE;
}
