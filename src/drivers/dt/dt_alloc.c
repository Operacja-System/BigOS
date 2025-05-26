#include <drivers/dt/dt_alloc.h>
#include <stdbigos/types.h>

// Ptr to start of arena area
static u32* arena_start = ((void*)0);

// Arena size in bytes
static u32 arena_size;

// Used area size in bytes
static u32 arena_offset;

// Helper for aligning to 4 bytes
u32 align4(u32 off) {
	return (off + 3) & ~3u;
}

// Helper for aligning to 32 bytes
u32 align32(u32 off) {
	return (off + 31) & ~31u;
}

int dt_arena_init(void* start, u32 size) {
	if (start == ((void*)0) || size == 0)
		return -1;

	arena_start = (u32*)start;
	arena_size = size;
	arena_offset = 0;

	return 0;
}

void* dt_alloc(u32 size) {
	// Idk if needed
	if (size == 0)
		return ((void*)0);

	// Align to 4 bytes
	u32 align = align4(size);

	if (arena_offset + align > arena_size)
		return ((void*)0);

	void* new_block = arena_start + arena_offset;
	arena_offset += align;

	return new_block;
}

void dt_arena_reset(void) {
	// Invalidates previously allocated blocks by ignoring them
	arena_offset = 0;
}
