#include "mm_types.h"

#include "klog.h"

u64 page_size_get_in_bytes(page_size_t ps) {
	switch (ps) {
	case PAGE_SIZE_4kB:   return 0x1000ull;
	case PAGE_SIZE_2MB:   return 0x200000ull;
	case PAGE_SIZE_1GB:   return 0x40000000ull;
	case PAGE_SIZE_512GB: return 0x8000000000ull;
	case PAGE_SIZE_256TB: return 0x1000000000000ull;
	default:              return 0;
	}
}

void log_virt_mem_region(const virt_mem_region_t* vmr) {
	KLOGLN_NOTE("addr range: %p-%p", vmr->addr, vmr->addr + vmr->size - 1);
	KLOGLN_NOTE("size: %lu", vmr->size);
	KLOGLN_NOTE("mapped: %b", vmr->mapped);
	if (vmr->mapped)
		KLOGLN_NOTE("map addr: 0x%lx", vmr->map_region.addr);
	const char* page_size_prefix[] = {"kilo", "mega", "giga", "tera", "peta"};
	KLOGLN_NOTE("page size: %s", page_size_prefix[vmr->ps]);
	char flags[5 + 1] = "-----";
	if (vmr->global)
		flags[0] = 'G';
	if (vmr->user)
		flags[1] = 'U';
	if (vmr->execute)
		flags[2] = 'X';
	if (vmr->write)
		flags[3] = 'W';
	if (vmr->read)
		flags[4] = 'R';
	KLOGLN_NOTE("flags: %s", flags);
	KLOGLN_NOTE("debug comment: %s", vmr->debug_comment);
}
