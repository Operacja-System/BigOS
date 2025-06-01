#ifndef DT_H
#define DT_H

#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

typedef struct dt_node_t dt_node_t;

// Initializes the device tree driver
// TODO: Currently parses the entire FDT at init, maybe change to lazy parsing later
// Builds the actual tree structure handleable by the driver, 0 if success, <0 if error
int dt_init(const void* fdt);

// Frees all memory used by the arena
void dt_cleanup(void);

// Find a node by full path in the tree, dt_node_t ptr if success, nullptr if error
dt_node_t* dt_node_find(const char* path);

// Get a node's name, const char ptr name if success, nullptr if error
const char* dt_node_get_name(const dt_node_t* node);

// Get node's child count, >=0 if success, <0 if error
int dt_node_child_count(const dt_node_t* node);

// Get a node's next sibling, dt_node_t ptr if success, nullptr if error or node is the last child
dt_node_t* dt_get_next_child(const dt_node_t* node);

// Returns the pointer to the root of the parsed device tree if success, nullptr if error
dt_node_t* dt_get_root(void);

void dt_reset_root(void);

#endif
