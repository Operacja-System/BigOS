#ifndef BIGOS_KERNEL_DEVTREE_BLOB_HEADER
#define BIGOS_KERNEL_DEVTREE_BLOB_HEADER

#include <stdbigos/types.h>
#include <stdbigos/error.h>

typedef struct {
	u32 magic;
	u32 totalsize;
	u32 off_dt_struct;
	u32 off_dt_strings;
	u32 off_mem_rsvmap;
	u32 version;
	u32 last_comp_version;
	u32 boot_cpuid_phys;
	u32 size_dt_strings;
	u32 size_dt_struct;
} dt_header_t;

[[nodiscard]] dt_header_t dt_read_header(const void* dtb_addr);
[[nodiscard]] error_t dt_validate_header(dt_header_t dt_header);

#endif
