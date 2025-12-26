#include <drivers/dt/dt.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

#include "dt_alloc.h"
#include "dt_defines.h"
#include "dt_node.h"
#include "dt_parser.h"

static dt_node_t* root_node = nullptr;

#define DT_ARENA_SIZE 32760
static u8 dt_arena_buffer[DT_ARENA_SIZE];

dt_node_t* dt_get_root(void) {
	return root_node;
}

void dt_cleanup(void) {
	dt_arena_reset();
	root_node = nullptr;
}

int dt_init(const void* fdt) {
	if (!fdt)
		return -1;

	u32 magic = read_be32(fdt);
	if (magic != FDT_MAGIC)
		return -1;

	u32 fdt_size = read_be32((u8*)fdt + FDT_OFF_TOTAL_SIZE);

	buffer_t fdt_buf = make_buffer(fdt, fdt_size);

	if (fdt_buf.size < (FDT_OFF_OFF_DT_STRINGS + 4))
		return -1;

	u32 total_size;
	u32 struct_off;
	u32 strings_off;
	u32 struct_size;
	u32 fdt_version;
	if (!buffer_read_u32_be(fdt_buf, FDT_OFF_TOTAL_SIZE, &total_size) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_DT_STRUCT, &struct_off) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_DT_STRINGS, &strings_off) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_SIZE_DT_STRUCT, &struct_size) ||
	    !buffer_read_u32_be(fdt_buf, FDT_OFF_VERSION, &fdt_version))
		return -2;

	if (fdt_version > FDT_COMPATIBLE_VERSION)
		return -3;

	if (struct_off + struct_size > total_size)
		return -3;

	if (!dt_arena_init(dt_arena_buffer, DT_ARENA_SIZE))
		return -4;

	u32 offset = struct_off + 4;
	root_node = parse_subtree(&fdt_buf, &offset, struct_off + struct_size, strings_off, nullptr);
	if (!root_node)
		return -5;

	return 0;
}
