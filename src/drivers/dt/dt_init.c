#include <drivers/dt/dt.h>
#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

#include "dt_alloc.h"
#include "dt_parser.h"

dt_node_t* root_node = nullptr;

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

// HACK: WARNING, the handling on big endian native machines not implemented, the second argument is ignored for now
int dt_init(buffer_t fdt_buf, [[maybe_unused]] endianness_t machine_big_endian) {
	if (!fdt_buf.data || fdt_buf.size < (FDT_OFF_OFF_DT_STRINGS + 4))
		return -1;

	u32 magic;
	if (buffer_read_u32_be(&fdt_buf, 0, &magic) != BUFFER_OK || magic != FDT_MAGIC)
		return -2;

	u32 total_size;
	if (buffer_read_u32_be(&fdt_buf, FDT_OFF_TOTAL_SIZE, &total_size) != BUFFER_OK)
		return -2;

	u32 struct_off;
	if (buffer_read_u32_be(&fdt_buf, FDT_OFF_OFF_DT_STRUCT, &struct_off) != BUFFER_OK)
		return -2;

	u32 strings_off;
	if (buffer_read_u32_be(&fdt_buf, FDT_OFF_OFF_DT_STRINGS, &strings_off) != BUFFER_OK)
		return -2;

	u32 struct_size;
	if (buffer_read_u32_be(&fdt_buf, FDT_OFF_SIZE_DT_STRUCT, &struct_size) != BUFFER_OK)
		return -2;

	if (struct_off + struct_size > total_size)
		return -3;

	if (dt_arena_init(dt_arena_buffer, DT_ARENA_SIZE) < 0)
		return -4;

	u32 offset = struct_off + 4;
	root_node = parse_subtree(&fdt_buf, &offset, struct_off + struct_size, strings_off, nullptr);
	if (!root_node)
		return -5;

	return 0;
}
