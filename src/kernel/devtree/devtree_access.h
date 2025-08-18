#ifndef BIGOS_KERNEL_DEVTREE_DEVTREE_ACCESS
#define BIGOS_KERNEL_DEVTREE_DEVTREE_ACCESS

#include "devtree/devtree_header.h"
#include "devtree_types.h"
#include <stdbigos/error.h>

typedef struct {
	dt_header_t header;
	const void* dtb_addr;
	bool valid;
	dt_blob_addr_t node_cursor;
	dt_blob_addr_t parent_node_cursor;
	dt_blob_addr_t prop_cursor;
} dt_interactive_entity_t;

error_t dtie_create(const void* dtb_addr, dt_interactive_entity_t* dtieOUT);
error_t dtie_destroy(dt_interactive_entity_t* dtie);

error_t dtie_goto_root_node(dt_interactive_entity_t* dtie);
error_t dtie_goto_next_node(dt_interactive_entity_t* dtie);
error_t dtie_goto_child_node(dt_interactive_entity_t* dtie);
error_t dtie_goto_parent_node(dt_interactive_entity_t* dtie);
error_t dtie_goto_next_property(dt_interactive_entity_t* dtie);

error_t dtie_get_node_name(const dt_interactive_entity_t* dtie, buffer_t* buffOUT);
error_t dtie_get_prop_name(const dt_interactive_entity_t* dtie, buffer_t* buffOUT);
error_t dtie_get_prop_data(const dt_interactive_entity_t* dtie, buffer_t* buffOUT);

#endif
