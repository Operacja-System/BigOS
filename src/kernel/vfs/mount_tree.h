#ifndef _MOUNT_TREE_
#define _MOUNT_TREE_

#include <stdbigos/types.h>

#include "vfs.h"

#define ERROR_MT_MOUNTPOINT_EXSITS              213721476969ull // randomly generated UID
#define ERROR_MT_TRIED_TO_ADD_EDGE_WHICH_EXISTS 10444484387115746230ull
#define ERROR_MT_UNKNOWN                        15645921380825118731ull

#define MT_MAX_CHILDREN 32
#define MT_MAX_NODES    1024

struct MT_EDGE;
struct MT_NODE;

typedef struct MT_EDGE {
	char* label;
	struct MT_NODE* to;
} MT_EDGE;

typedef struct MT_NODE {
	SERVICE_HANDLE service;
	MT_EDGE edges[MT_MAX_CHILDREN]; // TODO: change it to linked list (when allocator will be available)
	u16 edges_size;
} MT_NODE;

typedef struct MT_ADD_MOUNTPOINT_STATUS {
	MT_NODE* res;
	u64 err;
} MT_ADD_MOUNTPOINT_STATUS;

typedef struct MT_ADD_NODE_STATUS {
	MT_NODE* res;
	u64 err;
} MT_ADD_NODE_STATUS;

MT_NODE* mt_init();
bool mt_find_edge_by_label(MT_NODE* node, char* label, MT_NODE** out);
MT_ADD_NODE_STATUS mt_add_node(MT_NODE* node, char* label, SERVICE_HANDLE service);
MT_ADD_MOUNTPOINT_STATUS mt_add_mountpoint(MT_NODE* root, VFS_PATH* path, SERVICE_HANDLE service);

#endif
