#ifndef BIGOS_KERNEL_DEVTREE_BLOB_INTERACTIVE_ENTITY
#define BIGOS_KERNEL_DEVTREE_BLOB_INTERACTIVE_ENTITY

#include "header.h"
#include <stdbigos/error.h>
#include <stdbigos/buffer.h>

//NOTE: This limits device tree blob height
#define DTIE_STACK_SIZE 32

typedef u64 dt_blob_addr_t;

typedef struct {
	dt_header_t header;
	const void* dtb_addr;
	bool valid;
	dt_blob_addr_t node_cursor;
	dt_blob_addr_t prop_cursor;
	dt_blob_addr_t cursor_stack[DTIE_STACK_SIZE];
	u32 cursor_stack_pointer;
} dt_interactive_entity_t;

[[nodiscard]] error_t dtie_create(const void* dtb_addr, dt_interactive_entity_t* dtieOUT);
[[nodiscard]] error_t dtie_destroy(dt_interactive_entity_t* dtie);

[[nodiscard]] error_t dtie_goto_root_node(dt_interactive_entity_t* dtie);
[[nodiscard]] error_t dtie_goto_next_node(dt_interactive_entity_t* dtie);
[[nodiscard]] error_t dtie_goto_child_node(dt_interactive_entity_t* dtie);
[[nodiscard]] error_t dtie_goto_parent_node(dt_interactive_entity_t* dtie);
[[nodiscard]] error_t dtie_goto_next_prop(dt_interactive_entity_t* dtie);

[[nodiscard]] error_t dtie_get_node_name(const dt_interactive_entity_t* dtie, buffer_t* buffOUT);
[[nodiscard]] error_t dtie_get_prop_name(const dt_interactive_entity_t* dtie, buffer_t* buffOUT);
[[nodiscard]] error_t dtie_get_prop_data(const dt_interactive_entity_t* dtie, buffer_t* buffOUT);

#endif
