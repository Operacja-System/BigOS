#include "dt_alloc.h"

#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

// Arena proper
u8 dt_arena_buffer[DT_ARENA_SIZE];

static u32* arena_start = nullptr;

// Arena size in bytes
static u32 arena_size;

// Used area size in bytes
static u32 arena_offset;

int dt_arena_init(void* start, u32 size) {
	if (start == nullptr || size == 0)
		return -1;

	arena_start = (u32*)start;
	arena_size = size;
	arena_offset = 0;

	return 0;
}

void* dt_alloc(u32 size) {
	//  May not be needed
	if (size == 0)
		return nullptr;

	// Align to 4 bytes
	u32 align = alignu32(size, 4);

	if (arena_offset + align > arena_size)
		return nullptr;

	void* new_block = arena_start + arena_offset;
	arena_offset += align;

	return new_block;
}

// Invalidates previously allocated blocks by ignoring them
void dt_arena_reset(void) {
	arena_offset = 0;
}
