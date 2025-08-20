#include "header.h"

#include <stdbigos/buffer.h>

static constexpr u32 dt_magic = 0xd00dfeed;
static constexpr u32 dt_spec_version = 17;

dt_header_t dt_read_header(const void* dtb_addr) {
	dt_header_t dt_header = {0};
	buffer_t dt_header_buffer = make_buffer(dtb_addr, sizeof(dt_header_t));
	buffer_read_u32_be(dt_header_buffer, 0 * sizeof(u32), &dt_header.magic);
	buffer_read_u32_be(dt_header_buffer, 1 * sizeof(u32), &dt_header.totalsize);
	buffer_read_u32_be(dt_header_buffer, 2 * sizeof(u32), &dt_header.off_dt_struct);
	buffer_read_u32_be(dt_header_buffer, 3 * sizeof(u32), &dt_header.off_dt_strings);
	buffer_read_u32_be(dt_header_buffer, 4 * sizeof(u32), &dt_header.off_mem_rsvmap);
	buffer_read_u32_be(dt_header_buffer, 5 * sizeof(u32), &dt_header.version);
	buffer_read_u32_be(dt_header_buffer, 6 * sizeof(u32), &dt_header.last_comp_version);
	buffer_read_u32_be(dt_header_buffer, 7 * sizeof(u32), &dt_header.boot_cpuid_phys);
	buffer_read_u32_be(dt_header_buffer, 8 * sizeof(u32), &dt_header.size_dt_strings);
	buffer_read_u32_be(dt_header_buffer, 9 * sizeof(u32), &dt_header.size_dt_struct);
	return dt_header;
}

/// Public

error_t dt_validate_header(dt_header_t dt_header) {
	if (dt_header.magic != dt_magic)
		return ERR_NOT_VALID;
	if (dt_header.last_comp_version > dt_spec_version)
		return ERR_VERSION_NOT_COMPATIBLE;
	if (dt_header.off_dt_struct + dt_header.size_dt_struct > dt_header.totalsize)
		return ERR_NOT_VALID;
	if (dt_header.off_dt_strings + dt_header.size_dt_strings > dt_header.totalsize)
		return ERR_NOT_VALID;
	return ERR_NONE;
}
