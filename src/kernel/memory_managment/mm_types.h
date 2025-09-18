#ifndef BIGOS_KERNEL_MEMORY_MENAGMENT_MM_TYPES
#define BIGOS_KERNEL_MEMORY_MENAGMENT_MM_TYPES

#include <stdbigos/types.h>

typedef u64 vpn_t;
typedef u64 ppn_t;
typedef u64 phys_addr_t;

[[nodiscard]]
static inline phys_addr_t ppn_to_phys_addr(ppn_t ppn, u16 offset) {
	return (phys_addr_t)(ppn << 12) + (offset & 0xfff);
}

[[nodiscard]]
static inline ppn_t phys_addr_to_ppn(phys_addr_t addr) {
	return (ppn_t)(addr >> 12);
}

[[nodiscard]]
static inline void* vpn_to_virt_addr(vpn_t vpn, u16 offset) {
	return (void*)(vpn << 12) + (offset & 0xfff);
}

[[nodiscard]]
static inline vpn_t virt_addr_to_vpn(void* addr) {
	return (ppn_t)(addr) >> 12;
}

typedef enum : u8 {
	PAGE_SIZE_4kB = 0,   // kilo-page
	PAGE_SIZE_2MB = 1,   // mega-page
	PAGE_SIZE_1GB = 2,   // giga-page
	PAGE_SIZE_512GB = 3, // tera-page
	PAGE_SIZE_256TB = 4, // peta-page
} page_size_t;

[[nodiscard]]
u64 page_size_get_in_bytes(page_size_t ps);

typedef struct {
	phys_addr_t addr;
	size_t size;
} phys_mem_region_t;

typedef struct {
	bool mapped;
	bool read;
	bool write;
	bool execute;
	bool user;
	bool global;
	page_size_t ps;
	void* addr;
	size_t size;
	phys_mem_region_t map_region;
	const char* debug_comment;
} virt_mem_region_t;

void log_virt_mem_region(virt_mem_region_t* vmr);

#endif // !BIGOS_KERNEL_MEMORY_MENAGMENT_MM_TYPES
