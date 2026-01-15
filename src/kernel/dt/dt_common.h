#ifndef DT_COMMON_H
#define DT_COMMON_H

#include <stdbigos/types.h>

#include "dt.h"

// Offsets are of type u32

// Skip node's tag and name and return first aligned offset after them, >0 if success, 0 if error
u32 dt_skip_node_name(const void* fdt, dt_node_t node);

// Skip node's properties starting at offset (if they exist) and return first aligned offset after them
// >0 if success, 0 if error
u32 dt_skip_node_properties(const void* fdt, u32 offset);

// Skip node's tag, name and properties and return first aligned offset after them, >0 if success, 0 if error
u32 dt_skip_node_header(const void* fdt, dt_node_t node);

// Skip an overarching node's nested nodes starting at the start of the first nested node, >0 if success, 0 if error
u32 dt_skip_nested_nodes(const void* fdt, dt_node_t nested_node);

#endif // !DT_COMMON_H
