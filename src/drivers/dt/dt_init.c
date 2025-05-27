#include <drivers/dt/dt.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/types.h>

#include "dt_alloc.h"
#include "dt_parser.h"

struct dt_node* root_node = nullptr;

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

int dt_init(const void* fdt_blob, u32 blob_size) {
	if (!fdt_blob || blob_size < (FDT_OFF_OFF_DT_STRINGS + 4))
		return -1;

	const u8* fdt_u8 = (const u8*)fdt_blob;
	u32 magic = read_be32(fdt_u8 + FDT_OFF_MAGIC);
	if (magic != FDT_MAGIC)
		return -2;

	u32 total_size = read_be32(fdt_u8 + FDT_OFF_TOTAL_SIZE);
	u32 struct_off = read_be32(fdt_u8 + FDT_OFF_OFF_DT_STRUCT);
	u32 strings_off = read_be32(fdt_u8 + FDT_OFF_OFF_DT_STRINGS);

	u32 struct_size = read_be32(fdt_u8 + FDT_OFF_SIZE_DT_STRUCT);

	if (total_size > blob_size || struct_off + struct_size > blob_size)
		return -3;

	if (dt_arena_init(dt_arena_buffer, DT_ARENA_SIZE) < 0)
		return -4;

	u32 offset = struct_off + 4;

	root_node = parse_subtree(fdt_blob, &offset, struct_off + struct_size, strings_off, nullptr);
	if (!root_node)
		return -5;

	return 0;
}
