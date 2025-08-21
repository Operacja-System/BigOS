#ifndef BIGOS_KERNEL_DEVTREE_BLOB_INTERACTIVE_ENTITY
#define BIGOS_KERNEL_DEVTREE_BLOB_INTERACTIVE_ENTITY

#include <stdbigos/buffer.h>
#include <stdbigos/error.h>

#include "header.h"

// NOTE: This limits device tree blob height
#define DTBIE_STACK_SIZE 32
// As of device tree version 17
#define DTB_DEFAULT_ADDRESS_CELLS 2
#define DTB_DEFAULT_SIZE_CELLS 1
// Device tree does not specify max value but this OS cannot handle addresses bigger then u64_max
#define MAX_CELLS 2

typedef u64 dtb_addr_t;

typedef struct {
	dt_header_t header;
	const void* dtb_addr;
	bool valid;
	dtb_addr_t node_cursor;
	dtb_addr_t prop_cursor;
	dtb_addr_t cursor_stack[DTBIE_STACK_SIZE];
	u32 cursor_stack_pointer;
} dtb_interactive_entity_t; // Refered to as dtbie

[[nodiscard]] error_t dtbie_create(const void* dtb_addr, dtb_interactive_entity_t* dtbieOUT);
[[nodiscard]] error_t dtbie_destroy(dtb_interactive_entity_t* dtbie);

[[nodiscard]] error_t dtbie_get_prop_data(dtb_interactive_entity_t* dtbie, buffer_t* buffOUT);
[[nodiscard]] error_t dtbie_get_node_cells(dtb_interactive_entity_t* dtbie, u32* addrcellsOUT, u32* sizecellsOUT);

[[nodiscard]] error_t dtbie_goto_node(dtb_interactive_entity_t* dtbie, const char* path);
[[nodiscard]] error_t dtbie_goto_node_ignore_address(dtb_interactive_entity_t* dtbie, const char* path, u64 nodenum);
[[nodiscard]] error_t dtbie_count_children_nodes(dtb_interactive_entity_t* dtbie, const char* nodename, u64* countOUT);
[[nodiscard]] error_t dtbie_goto_prop(dtb_interactive_entity_t* dtbie, const char* propname);
[[nodiscard]] error_t dtbie_does_node_exist(dtb_interactive_entity_t* dtbie, const char* path);

#endif
