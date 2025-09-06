#ifndef _VFS_H_
#define _VFS_H_

#include <stdbigos/pstring.h>
#include <stdbigos/types.h>
#include <vfs/fs_server_protocol.h>

// DEBUG for testing CMake configuration
void vfsmain();
void vfs_init();

// Kernel pipe data, not managed by fs_server_protocol
typedef struct KernelPipe_t {
	u64 pipe_id;
	u64 attributes;
} KernelPipe_t;

// Maybe some metadata here?
typedef struct Service_t {
	pstring_t name;
	KernelPipe_t server_pipe;
} Service_t;

// Table of opened files
typedef struct FtEntry_t {
	bool isPipe;
	union {
		struct {
			Service_t server;
			FSFileHandle_t file_id;
		};
		KernelPipe_t kernel_pipe;
	};
	FSOpenMode_t mode;
} FtEntry_t;

/// Memory-owning representation of path
typedef pstring_t VfsPathBuff_t;

/// View of the VfsPathBuff
typedef struct {
	pstring_t path;
} VfsPath_t;

// TODO: implement
/// Normalizes the path and returns 'owning' reference
// VfsPathBuff_t vfs_path_buff(const pstring_t* path);

VfsPath_t vfs_path_new(const VfsPathBuff_t* path);

/// Iterates over subsequent path elements
bool vfs_path_next(VfsPath_t* path, pstring_t* edge);

#endif
