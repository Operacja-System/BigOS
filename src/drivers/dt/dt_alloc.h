#ifndef DT_ALLOC_H
#define DT_ALLOC_H

#include <stddef.h>

// Initialize the arena allocator with the start ptr at the start of the memory block of size bytes for allocations
// TODO: make rw buffer
bool dt_arena_init(void* start, size_t size);

// Allocate size bytes from the arena, ptr to start of the allocated block if succes, nullptr if error
// (e.g. out of arena memory)
void* dt_alloc(size_t size);

// Reset the arena allocator to reuse all memory, all previous allocators are invalidated
void dt_arena_reset(void);

#endif
