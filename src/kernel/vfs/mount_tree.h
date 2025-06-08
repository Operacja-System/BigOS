#ifndef _MOUNT_TREE_H_
#define _MOUNT_TREE_H_

#include <stdbigos/error.h>
#include <stdbigos/pstring.h>
#include <stdbigos/types.h>

#include "vfs.h"

typedef struct MtEdge_t {
	pstring_t label;
	struct MtNode_t* to;
} MtEdge_t;

typedef struct MtEdgeList_t {
	MtEdge_t edge;
	struct MtEdgeList_t* next;
} MtEdgeList_t;

typedef struct MtNode_t {
	ServiceHandle_t service;
	MtEdgeList_t* edge_list;
} MtNode_t;

/// Initializes the mount tree; does basic setup
error_t mt_init(MtNode_t** res);

/// Deallocate the MtNode subtree
void mt_free(MtNode_t* node);

/// Attempts to step over `label` to next node.
bool mt_step(const MtNode_t* node, const pstring_t* label, MtNode_t** out);

/// Attempts to walk `path` starting from `node`.
/// On fail returns false and `path` is modified to point to not-walked sufix
bool mt_walk(const MtNode_t* node, VfsPath_t* path, MtNode_t** out);

/// Ads a new node for handling a service below `node` accessible via `label`
// error_t mt_add_node(MtNode_t* node, pstring_t label, ServiceHandle_t service, MtNode_t** new_node);

error_t mt_add_mountpoint(MtNode_t* root, VfsPath_t path, ServiceHandle_t service, MtNode_t** out);

#endif
