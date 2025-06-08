#include "vfs_alloc.h"

#include <stdbigos/string.h>
#include <stdbigos/types.h>

#define ALIGNMENT 16

// Static memory arena for allocator
static u8 ARENA[VFS_ALLOCATOR_SIZE] __attribute__((aligned(ALIGNMENT)));
static size_t POSITION = 0;

static size_t next_aligned_position(size_t position) {
	size_t position_bump = (position & (ALIGNMENT - 1)) ? ALIGNMENT : 0;
	return position + position_bump;
}

void* vfs_malloc(size_t size) {
	void* res = ARENA + POSITION;
	POSITION = next_aligned_position(POSITION + size);
	return res;
}

void* vfs_realloc(void* ptr, size_t new_size) {
	void* new_memory = vfs_malloc(new_size);
	if (ptr == nullptr) {
		return new_memory;
	}

	// Yes, this goes out of bounds for `ptr`, but I do not care
	memcpy(new_memory, ptr, new_size);
	return new_memory;
}

void vfs_free(void* ptr [[maybe_unused]]) {
	// Yes, much eficiency
}
