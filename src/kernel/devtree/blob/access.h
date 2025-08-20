#ifndef BIGOS_KERNEL_DEVTREE_BLOB_ACCESS
#define BIGOS_KERNEL_DEVTREE_BLOB_ACCESS

#include <stdbigos/buffer.h>

[[nodiscard]] error_t dtb_get_prop_data(const void* dtb, const char* path, const char* prop_name, bool ignore_address,
                                        buffer_t* buffOUT);
[[nodiscard]] error_t dtb_does_node_exist(const void* dtb, const char* path, bool ignore_address);
[[nodiscard]] error_t dtb_count_nodes_ignore_address(const void* dtb, const char* path, const char* name, u64* cOUT);

#endif
