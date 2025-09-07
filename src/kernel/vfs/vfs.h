#ifndef _VFS_H_
#define _VFS_H_

#include <stdbigos/pstring.h>
#include <stdbigos/types.h>
#include <vfs/fs_server_protocol.h>

#include "pipes.h"

// DEBUG for testing CMake configuration
void vfsmain();
void vfs_init();

/// Represents a driver service
typedef struct Service_t {
	pstring_t name;
	KernelPipe_t server_pipe;
} Service_t;

/// Represents an open file.
/// This is either a pipe or an entry managed via fs_server_protocol
typedef struct FtEntry_t {
	enum FtEntryType {
		FT_ENTRY_FILE,
		FT_ENTRY_READ_PIPE,
		FT_ENTRY_WRITE_PIPE,
		FT_ENTRY_PIPE,
	} entry_type;
	union {
		// TODO: Extract this into separate struct so a single file_handle
		//       could be used by multiple processes (ex. after fork)
		struct {
			Service_t server;
			FSFileHandle_t file_handle;
		};
		KernelReadPipe_t kernel_read_pipe;
		KernelWritePipe_t kernel_write_pipe;
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
