#ifndef DT_H
#define DT_H

// TODO: Change return values of functions that return e.g. no next properties and errors as the same

#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

// A node is an offset from the start of fdt with FDT_BEGIN_NODE
typedef u32 dt_node_t;

// A property is an offset from the start of fdt with FDT_PROP
typedef u32 dt_prop_t;

// Caches fdt properties and sets root_node for faster future parsing, 0 if success, <0 if error
int dt_init(const void* fdt);

// Reset properties of the currently used fdt
void dt_reset(void);

// Returns the shortened path, while outputting the name with a pointer to it's start and length, nullptr if error
const char* dt_walk(const char* path, const char** name, u32* name_len);

// Get a node in a subtree of the fdt, leave as 0 for global search, >0 if success, 0 if error
dt_node_t dt_get_node_in_subtree_by_path(const void* fdt, dt_node_t node, const char* node_path);

// Get a node globally in the fdt, >0 if success, 0 if error
dt_node_t dt_get_node_by_path(const void* fdt, const char* node_path);

// Get a node's first child, 0 if no children exist or error, >0 if success
dt_node_t dt_get_node_child(const void* fdt, dt_node_t node);

// Get a node's next sibling, 0 if no siblings exist or error, >0 if success
dt_node_t dt_get_node_sibling(const void* fdt, dt_node_t node);

// Get a node's name as a buffer, valid buffer if success, invalid buffer if error
buffer_t dt_get_node_name(const void* fdt, dt_node_t node);

// Get a node's prop_name property, >0 if success, 0 if error
dt_prop_t dt_get_prop_by_name(const void* fdt, dt_node_t node, const char* prop_name);

// Get a node's first property, 0 if no properties exist or error, >0 if success
dt_prop_t dt_get_first_prop(const void* fdt, dt_node_t node);

// Get a node's next property after prop, 0 if no next properties exist or error
// >0 if success
dt_prop_t dt_get_next_prop(const void* fdt, dt_prop_t prop);

// Get a prop's name as a buffer, valid buffer if success, invalid buffer if error
buffer_t dt_get_prop_name(const void* fdt, dt_prop_t prop);

// Get a buffer for prop's data
buffer_t dt_get_prop_buffer(const void* fdt, dt_prop_t prop);

// Get node's child count, >=0 if success, <0 if error
int dt_node_child_count(const dt_node_t node);

// Get a node's next sibling, >0 if success, 0 if error or last child
dt_node_t dt_get_next_child(const dt_node_t node);

// Returns the node of the root of current fdt, >0 if success, 0 if error
dt_node_t dt_get_root(void);

// Getters for device tree parameters
u32 dt_get_total_size(void);

u32 dt_get_struct_off(void);

u32 dt_get_strings_off(void);

u32 dt_get_struct_size(void);

u32 dt_get_fdt_version(void);

#endif // !DT_H
