#include <debug/debug_stdio.h>

#include "pipes.h"
#include "vfs.h"
#include "vfs_alloc.h"

FtEntry_t* ft_add_entry() {
	// No check as this is just a mock.
	return (FtEntry_t*)vfs_malloc(sizeof(FtEntry_t));
}

void ft_free_entry(FtEntry_t* entry) {
	switch (entry->entry_type) {
	case FT_ENTRY_FILE:
		// TODO: implement.
		break;
	case FT_ENTRY_PIPE:
		pipe_close_read(&entry->kernel_pipe.read_end);
		pipe_close_write(&entry->kernel_pipe.write_end);
		break;
	case FT_ENTRY_READ_PIPE:  pipe_close_read(&entry->kernel_read_pipe); break;
	case FT_ENTRY_WRITE_PIPE: pipe_close_write(&entry->kernel_write_pipe);
	}

	vfs_free(entry);
}
