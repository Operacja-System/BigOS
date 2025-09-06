#include "vfs.h"

#include <debug/debug_stdio.h>
#include <debug/panic.h>
#include <stdbigos/assert.h>
#include <stdbigos/error.h>
#include <stdbigos/pstring.h>
#include <stdbigos/string.h>
#include <stddef.h>

#include "mount_tree.h"
#include "pipes.h"
#include "vfs_alloc.h"

// Here just for debugging
void vfsmain() {
	DEBUG_PUTS("Hello VFS\n");
	pstring_t path1 = ERRX_UNWRAP(pstring_l2w("/foo/bar/baz/file.c"));
	VfsPath_t path = vfs_path_new(&path1);

	pstring_t edge = {.len = 0, .data = nullptr};
	size_t i = 0;
	while (vfs_path_next(&path, &edge)) {
		if (edge.data == nullptr) {
			PANIC("vfs_path_next returned null edge data");
		}
		DEBUG_PRINTF("Path part %zu: ", i++);
		for (size_t i = 0; i < edge.len; i++) {
			DEBUG_PUTC(edge.data[i]);
		}
		DEBUG_PUTC('\n');
	}

	ServiceHandle_t example_driver;

	FtEntry_t* example_driver_fd1;
	FtEntry_t* example_driver_fd2;
	char example_driver_name[] = "example driver";
	pstring_t example_driver_name_pstring = pstring_l2w(example_driver_name).val;

	pipe_create(example_driver_name_pstring, &example_driver);
	char buff[100];

	char example_message1[] = "Hello server-fs! I'm example driver.\n";
	pipe_open(example_driver->pipe_id, &example_driver_fd1);
	pipe_write(example_driver_fd1->file_id, strlen(example_message1) + 1, (u8*)example_message1);
	pipe_read(example_driver_fd1->file_id, strlen(example_message1) + 1, (u8*)buff);
	DEBUG_PUTS(buff);

	char example_message2[] = "Hi there! I'm example driver too!\n";
	pipe_open(example_driver->pipe_id, &example_driver_fd2);
	pipe_write(example_driver_fd2->file_id, strlen(example_message2) + 1, (u8*)example_message2);
	pipe_read(example_driver_fd2->file_id, strlen(example_message2) + 1, (u8*)buff);
	DEBUG_PUTS(buff);

	pipe_remove(example_driver_fd1->file_id);
	if (pipe_read(example_driver_fd2->file_id, strlen(example_message2) + 1, (u8*)buff) == ERR_BROKEN_FILE_DESCRIPTOR) {
		DEBUG_PUTS("File descriptor broken (ok)\n");
	} else {
		DEBUG_PUTS("File descriptor not broken (not ok)\n");
	}
}

VfsPath_t vfs_path_new(const VfsPathBuff_t* path_buff) {
	return (VfsPath_t){.path = *path_buff};
}

bool vfs_path_next(VfsPath_t* path, pstring_t* edge) {
	if (!path || pstring_len_silenterr(&path->path) == 0) {
		return false;
	}

	const pstring_t vpath = path->path;
	pstring_t split = ERRX_UNWRAP(pstring_strchr(&vpath, '/'));
	if (split.len == 0) {
		path->path.data = nullptr;
		path->path.len = 0;
		*edge = vpath;
	}

	size_t split_pos = vpath.len - split.len;
	*edge = ERRX_UNWRAP(pstring_slice_view(&vpath, 0, split_pos));
	path->path = ERRX_UNWRAP(pstring_slice_view(&vpath, split_pos + 1, vpath.len));
	return true;
}
