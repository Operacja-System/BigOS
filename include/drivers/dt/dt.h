#ifndef DT_H
#define DT_H

#include <drivers/dt/dt_node.h>
#include <stdbigos/types.h>

#define DT_ARENA_SIZE 16384

extern u8 dt_arena_buffer[DT_ARENA_SIZE];

// Root of the parsed device tree
extern struct dt_node* root_node;

// Initializes the device tree driver
// TODO: Currently parses the entire FDT at init, maybe change to lazy parsing later
// Builds the actual tree structure handleable by the driver, 0 if success, <0 if error
int dt_init(const void* fdt_blob, u32 blob_size);

// Frees all memory used by the arena
void dt_cleanup(void);

// Find a node by full path in the tree, dt_node ptr if success, nullptr if error
struct dt_node* dt_find_node(const char* path);

// Returns the pointer to the root of the parsed device tree if success, nullptr if error
struct dt_node* dt_get_root(void);

#endif
