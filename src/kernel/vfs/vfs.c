#include "vfs.h"

#include <debug/debug_stdio.h>
#include <debug/panic.h>
#include <stdbigos/assert.h>
#include <stdbigos/error.h>
#include <stdbigos/pstring.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stddef.h>

#include "file_table.h"
#include "pipes.h"
#include "vfs_alloc.h"

static void vfs_test_path_next() {
	pstring_t path1 = ERRX_UNWRAP(pstring_l2w("/foo/bar/baz/file.c"));
	VfsPath_t path = vfs_path_new(&path1);

	pstring_t edge = {.len = 0, .data = nullptr};
	size_t i = 0;
	while (vfs_path_next(&path, &edge)) {
		if (edge.data == nullptr) {
			PANIC("vfs_path_next returned null edge data");
		}
		DEBUG_PRINTF("Path part %zu: ", i++);
		DEBUG_PUTPS(&edge);
		DEBUG_PUTC('\n');
	}
}

static void vfs_test_pipes() {
	FtEntry_t* example_file_entry1;
	FtEntry_t* example_file_entry2;

	// Find a space to place these structs in user-process-memory
	example_file_entry1 = ft_add_entry();
	example_file_entry2 = ft_add_entry();

	// Create a pipe that connects these file entries
	kassert(pipe_create(&example_file_entry1->kernel_read_pipe, &example_file_entry2->kernel_write_pipe) == ERR_NONE);

	const pstring_t message1 =
	    ERRX_UNWRAP(pstring_l2w("Hello from vfs! This message is brought to you via a Kernel Pipe.\n"));
	const pstring_t message2 =
	    ERRX_UNWRAP(pstring_l2w("Hello again, it seems that these pipes are not just for one time use!\n"));
	const pstring_t message3 = ERRX_UNWRAP(pstring_l2w("How convenient!\n"));

	// TODO: Figure out better way of doing 'this' with pstring
	pstring_t buff = (pstring_t){
	    .len = 100,
	    .data = vfs_alloca(100),
	};

	// Write and read a mesage
	kassert(ERRX_UNWRAP(pipe_write(&example_file_entry2->kernel_write_pipe, &message1)) == message1.len);
	kassert(ERRX_UNWRAP(pipe_read(&example_file_entry1->kernel_read_pipe, &buff)) == message1.len);

	// Normally one would collect the size from pipe_read. Here we assert so the size is known.
	buff.len = message1.len;
	DEBUG_PUTPS(&buff);

	// Create a buffer too small to read whole next message
	buff.len = 20;

	// Write two messages at once and then read them with different sizes
	kassert(ERRX_UNWRAP(pipe_write(&example_file_entry2->kernel_write_pipe, &message2)) == message2.len);
	kassert(ERRX_UNWRAP(pipe_read(&example_file_entry1->kernel_read_pipe, &buff)) == 20);
	DEBUG_PUTPS(&buff);

	// This buffer will fit the rest of the message
	buff.len = 100;

	kassert(ERRX_UNWRAP(pipe_write(&example_file_entry2->kernel_write_pipe, &message3)) == message3.len);
	kassert(ERRX_UNWRAP(pipe_read(&example_file_entry1->kernel_read_pipe, &buff)) == message2.len + message3.len - 20);

	buff.len = message2.len + message3.len - 20;
	DEBUG_PUTPS(&buff);

	// Reading on an empty buffer should not read characters
	kassert(ERRX_UNWRAP(pipe_read(&example_file_entry1->kernel_read_pipe, &buff)) == 0);

	ft_free_entry(example_file_entry1);
	ft_free_entry(example_file_entry2);
}

// Here just for debugging
void vfsmain() {
	DEBUG_PUTS("Hello VFS\n");

	DEBUG_PUTS("-> VFS showcase vfs_path_next:\n");
	vfs_test_path_next();
	DEBUG_PUTS("-> VFS showcase pipes:\n");
	vfs_test_pipes();

	DEBUG_PUTS("-> ALL vfs samples finished\n");
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
