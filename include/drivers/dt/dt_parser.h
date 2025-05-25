#ifndef DT_PARSER_H
#define DT_PARSER_H

#include <stdbigos/types.h>

u32 read_be32(const void* addr);

void test();

struct dt_prop* parse_props(const void* fdt, u32 props_offset, u32 props_size);

struct dt_node* parse_subtree(const void* fdt, u32* offset, u32 max_offset, struct dt_node* parent);

#endif
