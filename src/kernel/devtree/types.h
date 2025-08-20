#ifndef BIGOS_KERNEL_DEVTREE_DEVTREE_TYPES
#define BIGOS_KERNEL_DEVTREE_DEVTREE_TYPES

#include <stdbigos/types.h>
#include <stdbigos/buffer.h>

typedef struct {
	const char* name;
	buffer_t data;
	struct dt_prop_t* next_prop;
} dt_prop_t;

typedef struct {
	const char* name;
	struct dt_node_t* next_node;
	struct dt_node_t* child;
	dt_prop_t* prop;
} dt_node_t;

#endif
