#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

#include "dt.h"
#include "dt_defines.h"

// Properties of the currently used fdt, in the future maybe use malloc for allocating and deallocating them?
static u32 total_size = 0;
static u32 struct_off = 0;
static u32 strings_off = 0;
static u32 struct_size = 0;
static u32 fdt_version = 0;

// root_node is a static marker to not retread the header
static dt_node_t root_node = 0;

dt_node_t dt_get_root(void) {
	return root_node;
}

u32 dt_get_total_size(void) {
	return total_size;
}

u32 dt_get_struct_off(void) {
	return struct_off;
}

u32 dt_get_strings_off(void) {
	return strings_off;
}

u32 dt_get_struct_size(void) {
	return struct_size;
}

u32 dt_get_fdt_version(void) {
	return fdt_version;
}

// Read the header to cache properties
int dt_init(const void* fdt) {
	if (!fdt)
		return -1;

	u32 magic = read_be32(fdt);
	if (magic != FDT_MAGIC)
		return -1;

	u32 fdt_size = read_be32((u8*)fdt + FDT_OFF_TOTAL_SIZE);

	// Temporary buffer to read properties more easily
	buffer_t fdt_buf = make_buffer(fdt, fdt_size);

	if (fdt_buf.size < (FDT_OFF_OFF_DT_STRINGS + 4))
		return -1;

	if (!buffer_read_u32_be(fdt_buf, FDT_OFF_TOTAL_SIZE, &total_size) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_DT_STRUCT, &struct_off) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_DT_STRINGS, &strings_off) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_SIZE_DT_STRUCT, &struct_size) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_VERSION, &fdt_version))
		return -2;

	if (fdt_version > FDT_COMPATIBLE_VERSION)
		return -3;

	if (struct_off + struct_size > total_size)
		return -4;

	root_node = struct_off;

	return 0;
}

// Reset properties
void dt_reset(void) {
	total_size = 0;
	struct_off = 0;
	strings_off = 0;
	struct_size = 0;
	fdt_version = 0;

	root_node = 0;
}
