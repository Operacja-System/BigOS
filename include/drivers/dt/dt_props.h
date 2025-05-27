#ifndef DT_PROPS_H
#define DT_PROPS_H

#include <stdbigos/types.h>

struct dt_node;
struct dt_prop;

// Find a property of node by a name, dt_prop ptr if success, nullptr if error
struct dt_prop* dt_find_prop(const struct dt_node* node, const char* name);

// Read a single 32-bit value from a node's name prop and write it to out, 0 if success, <0 if error
int dt_prop_read_u32(const struct dt_node* node, const char* name, u32* out);

// Read a single 64-bit value from a node's name prop and write it to out, 0 if success, <0 if error
int dt_prop_read_u64(const struct dt_node* node, const char* name, u64* out);

// Read a single 32-bit value from a node's name prop's id index and write it to out, 0 if success, <0 if error
int dt_prop_read_by_id_u32(const struct dt_node* node, const char* name, u32 index, u32* out);

// Read an array of up to out_size node's name prop values and write them to out
// number of successfully read values if success, <0 if error
int dt_prop_read_array_u32(const struct dt_node* node, const char* name, u32 out[], u32 out_size);

// Read a null-terminated string from node's name prop, const char ptr if succes, nullptr if error
const char* dt_prop_read_str(const struct dt_node* node, const char* name);

// Read an array of up to out_size node's name prop strings and write them to out
// number of successfully read strings if success, <0 if error
int dt_prop_read_array_str(const struct dt_node* node, const char* name, const char* out[], u32 out_size);

// Read a bool ean value of node's name prop, 1 if exists or prop empty, 0 if doesn't exist, -1 if error
int dt_prop_read_bool(const struct dt_node* node, const char* name);

// Read a node's name phandle (a handle to a different node) and write it to out, 0 if success, -1 if error
int dt_prop_read_phandle(const struct dt_node* node, const char* name, u32* out);

// Print the node's properties
void dt_print_props(const struct dt_node* node, u8 depth);

// Print the entire subtree with properties
void dt_print_tree(const struct dt_node* node, u8 depth);

#endif
