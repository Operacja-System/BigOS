#ifndef DT_PROPS_H
#define DT_PROPS_H

#include <drivers/dt/dt_node.h>
#include <stdbigos/types.h>

// Can be used to check for bool-type props
// Find a property of node by a name, dt_prop_t ptr if success, nullptr if error
dt_prop_t* dt_find_prop(const dt_node_t* node, const char* name);

// Get a buffer of node's name value, the buffer's BUFFER_ERROR field will have any potential errors
buffer_t dt_prop_get_buffer(const dt_node_t* node, const char* name);

// Print the node's properties
void dt_print_props(const dt_node_t* node, u8 depth);

// Print the entire subtree with properties
void dt_print_tree(const dt_node_t* node, u8 depth);

#endif
