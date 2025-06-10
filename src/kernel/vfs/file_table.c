#include <debug/debug_stdio.h>

#include "vfs.h"
#include "vfs_alloc.h"

FtEntry_t* ft_add_entry(ServiceHandle_t handle, u64 file_id, int attributes) {
	FtEntry_t* ret = (FtEntry_t*)vfs_malloc(sizeof(FtEntry_t));
	ret->handle = handle;
	ret->file_id = file_id;
	ret->attributes = attributes;
	return ret;
}

// There could be more stuff here in future
void ft_free_entry(FtEntry_t* entry) {
	vfs_free(entry);
}
