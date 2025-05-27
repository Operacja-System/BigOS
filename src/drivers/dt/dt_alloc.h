#ifndef DT_ALLOC_H
#define DT_ALLOC_H

#include <stdbigos/types.h>

#define DT_ARENA_SIZE 32760
extern u8 dt_arena_buffer[DT_ARENA_SIZE];

extern struct dt_node* root_node;

// Initialize the arena allocator with the start ptr at the start of the memory block of size bytes for allocations
int dt_arena_init(void* start, u32 size);

// Allocate size bytes from the arena, ptr to start of the allocated block if succes, nullptr if error
// (e.g. out of arena memory)
void* dt_alloc(u32 size);

// Reset the arena allocator to reuse all memory, all previous allocators are invalidated
void dt_arena_reset(void);

#endif
