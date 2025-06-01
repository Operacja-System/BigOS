#include "dt_alloc.h"

#include <stdbigos/bitutils.h>
#include <stdbigos/types.h>

// Arena proper
static u32* arena_start = nullptr;

// Arena size in bytes
static size_t arena_size;

// Used area size in bytes
static size_t arena_offset;

u8* dt_get_arena_buffer(void) {
	return dt_arena_buffer;
}

// Helper for aligning to 32 bytes
u32 align32(u32 off) {
	return (off + 31) & ~31u;
}

int dt_arena_init(void* start, u32 size) {
	if (start == nullptr || size == 0)
		return false;

	arena_start = (u32*)start;
	arena_size = size;
	arena_offset = 0;

	return true;
}

void* dt_alloc(size_t size) {
	//  May not be needed
	if (size == 0)
		return nullptr;

	// Align to 4 bytes
	u32 align = align_u32(size, 4);

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
