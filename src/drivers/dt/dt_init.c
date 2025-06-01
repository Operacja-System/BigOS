#include <drivers/dt/dt.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

#include "dt_alloc.h"
#include "dt_parser.h"

static dt_node_t* root_node = nullptr;

dt_node_t* dt_get_root(void) {
	return root_node;
}

void dt_reset_root(void) {
	root_node = nullptr;
}

#define FDT_MAGIC                 0xd00dfeed
#define FDT_OFF_MAGIC             0x00
#define FDT_OFF_TOTAL_SIZE        0x04
#define FDT_OFF_OFF_DT_STRUCT     0x08
#define FDT_OFF_OFF_DT_STRINGS    0x0C
#define FDT_OFF_MEM_RSVMAP        0x10
#define FDT_OFF_VERSION           0x14
#define FDT_OFF_LAST_COMP_VERSION 0x18
#define FDT_OFF_BOOT_CPUID_PHYS   0x1C
#define FDT_OFF_SIZE_DT_STRINGS   0x20
#define FDT_OFF_SIZE_DT_STRUCT    0x24

// TODO: implement reading dependent on endianness of the machine
int dt_init(const void* fdt, [[maybe_unused]] endianness_t machine_big_endian) {
	u32 magic = read_be32(fdt);
	if (magic != FDT_MAGIC)
		return -1;

	u32 fdt_size = read_be32((u8*)fdt + 4);

	buffer_t fdt_buf = make_buffer(fdt, fdt_size);

	if (fdt_buf.size < (FDT_OFF_OFF_DT_STRINGS + 4))
		return -1;

	u32 total_size;
	if (buffer_read_u32_be(fdt_buf, FDT_OFF_TOTAL_SIZE, &total_size) != BUFFER_OK)
		return -2;

	u32 struct_off;
	if (buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_DT_STRUCT, &struct_off) != BUFFER_OK)
		return -2;

	u32 strings_off;
	if (buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_DT_STRINGS, &strings_off) != BUFFER_OK)
		return -2;

	u32 struct_size;
	if (buffer_read_u32_be(fdt_buf, FDT_OFF_SIZE_DT_STRUCT, &struct_size) != BUFFER_OK)
		return -2;

	if (struct_off + struct_size > total_size)
		return -3;

	u8* dt_arena_buffer = dt_get_arena_buffer();

	if (dt_arena_init(dt_arena_buffer, DT_ARENA_SIZE) < 0)
		return -4;

	u32 offset = struct_off + 4;
	root_node = parse_subtree(&fdt_buf, &offset, struct_off + struct_size, strings_off, nullptr);
	if (!root_node)
		return -5;

	return 0;
}
