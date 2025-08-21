#ifndef BIGOS_KERNEL_DEVTREE_BLOB_INTERACTIVE_ENTITY_INTERNAL
#define BIGOS_KERNEL_DEVTREE_BLOB_INTERACTIVE_ENTITY_INTERNAL

#include "interactive_entity.h"
#include <stdbigos/error.h>

// After more loop interations that this the loop is considered infinite.
// It also acts as the limit of tokens inside dtb.
#define DTBIE_LOOP_LIMIT 5000

typedef enum : u32 {
	DTT_BEGIN_NODE = 0x1,
	DTT_END_NODE = 0x2,
	DTT_PROP = 0x3,
	DTT_NOP = 0x4,
	// yup, there is a gap here
	DTT_END = 0x9,
} dtb_token_t;

typedef struct {
	u32 len;
	u32 nameof;
} dtb_prop_header_t;

[[nodiscard]] error_t dtbie_goto_root_node(dtb_interactive_entity_t* dtbie);
[[nodiscard]] error_t dtbie_goto_next_node(dtb_interactive_entity_t* dtbie);
[[nodiscard]] error_t dtbie_goto_child_node(dtb_interactive_entity_t* dtbie);
[[nodiscard]] error_t dtbie_goto_parent_node(dtb_interactive_entity_t* dtbie);
[[nodiscard]] error_t dtbie_goto_next_prop(dtb_interactive_entity_t* dtbie);

[[nodiscard]] error_t dtbie_get_node_name(dtb_interactive_entity_t* dtbie, buffer_t* buffOUT);
[[nodiscard]] error_t dtbie_get_prop_name(dtb_interactive_entity_t* dtbie, buffer_t* buffOUT);

#endif
