#ifndef DT_NODE_H
#define DT_NODE_H

#include <stdbigos/types.h>

struct dt_prop {
	const char* name;
	const void* value;
	u32 data_length;
	struct dt_prop* next_prop;
};

struct dt_node {
	const char* name;
	u32 phandle;
	struct dt_prop* props;
	struct dt_node* parent;
	struct dt_node* first_child;
	struct dt_node* next_sibling;
};
// Both prop and node structures are lists with outside pointers only to the first element

#endif
