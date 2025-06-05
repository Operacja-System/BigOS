#ifndef DT_PROPS_H
#define DT_PROPS_H

#include <stdbigos/types.h>

#include "dt.h"

// Get a buffer of node's name value (buffer will be invalid if property with given name doesn't exit)
buffer_t dt_prop_get(const dt_node_t* node, const char* name);

// Print the node's properties
void dt_print_props(const dt_node_t* node, u8 depth);

// Print the entire subtree with properties
void dt_print_tree(const dt_node_t* node, u8 depth);

#endif
