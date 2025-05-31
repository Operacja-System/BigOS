#ifndef DT_NODE_H
#define DT_NODE_H

#include <stdbigos/types.h>

// Both prop and node structures are lists with outside pointers only to the first element
typedef struct dt_prop_t {
	const char* name;
	const void* value;
	u32 data_length;
	struct dt_prop_t* next_prop;
} dt_prop_t;

typedef struct dt_node_t {
	const char* name;
	u32 phandle;
	struct dt_prop_t* props;
	struct dt_node_t* parent;
	struct dt_node_t* first_child;
	struct dt_node_t* next_sibling;
} dt_node_t;

#endif
