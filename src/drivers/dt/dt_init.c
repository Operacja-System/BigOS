#include <drivers/dt/dt.h>
#include <drivers/dt/dt_alloc.h>
#include <drivers/dt/dt_parser.h>
#include <stdbigos/types.h>

#define FDT_MAGIC          0xd00dfeed
#define FDT_OFF_MAGIC      0x00
#define FDT_OFF_TOTAL_SIZE 0x04
#define FDT_OFF_DT_STRUCT  0x08
#define FDT_OFF_DT_STRINGS 0x10

int dt_init(const void* fdt_blob, u32 blob_size) {
	if (!fdt_blob || blob_size < (FDT_OFF_DT_STRINGS + 4))
		return -1;

	const u32* fdt_u32 = (const u32*)fdt_blob;
	u32 magic = read_be32(fdt_u32 + FDT_OFF_MAGIC);
	if (magic != FDT_MAGIC)
		return -2;

	u32 total_size = read_be32(fdt_u32 + FDT_OFF_TOTAL_SIZE);
	u32 struct_off = read_be32(fdt_u32 + FDT_OFF_DT_STRUCT);
	// TODO: Check if this works
	u32 struct_size = read_be32((const u8*)fdt_blob + FDT_OFF_DT_STRINGS) - struct_off;

	if (total_size > blob_size || struct_off + struct_size > blob_size)
		return -3;

	if (dt_arena_init(dt_arena_buffer, DT_ARENA_SIZE) < 0)
		return -4;

	u32 offset = struct_off;
	root_node = parse_subtree(fdt_blob, &offset, struct_off + struct_size, ((void*)0));
	if (!root_node)
		return -5;

	return 0;
}
