#ifndef DT_PARSER_H
#define DT_PARSER_H

#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

struct dt_prop* parse_props(buffer_t* fdt_buf, u32 props_offset, u32 props_size, u32 str_offset);

struct dt_node* parse_subtree(buffer_t* fdt_buf, u32* offset, u32 max_offset, u32 str_offset, struct dt_node* parent);

#endif
