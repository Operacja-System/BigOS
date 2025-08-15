#ifndef DT_PARSER_H
#define DT_PARSER_H

#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

#include "dt_node.h"

dt_prop_t* parse_props(const buffer_t* fdt_buf, u32 props_offset, u32 props_size, u32 str_offset);

dt_node_t* parse_subtree(const buffer_t* fdt_buf, u32* offset, u32 max_offset, u32 str_offset, dt_node_t* parent);

#endif
