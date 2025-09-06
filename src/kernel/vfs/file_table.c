#include <debug/debug_stdio.h>

#include "vfs.h"
#include "vfs_alloc.h"

FtEntry_t* ft_add_entry() {
	// No check as this is just a mock.
	return (FtEntry_t*)vfs_malloc(sizeof(FtEntry_t));
}

void ft_free_entry(FtEntry_t* entry) {
	vfs_free(entry);
}
