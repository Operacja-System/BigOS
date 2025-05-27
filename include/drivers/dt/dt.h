#ifndef DT_H
#define DT_H

#include <stdbigos/types.h>

struct dt_node;
struct dt_prop;

// Initializes the device tree driver
// TODO: Currently parses the entire FDT at init, maybe change to lazy parsing later
// Builds the actual tree structure handleable by the driver, 0 if success, <0 if error
int dt_init(const void* fdt_blob, u32 blob_size);

// Frees all memory used by the arena
void dt_cleanup(void);

// Find a node by full path in the tree, dt_node ptr if success, nullptr if error
struct dt_node* dt_node_find(const char* path);

// Get a node's name, const char ptr name if success, nullptr if error
const char* dt_node_get_name(const struct dt_node* node);

// Get node's child count, >=0 if success, <0 if error
int dt_node_child_count(const struct dt_node* node);

// Get a node's index-th in order child, dt_node ptr if success, nullptr if error
struct dt_node* dt_node_get_child(const struct dt_node* node, int index);

// Returns the pointer to the root of the parsed device tree if success, nullptr if error
struct dt_node* dt_get_root(void);

#endif
