#ifndef _VFS_ALLOC_H_
#define _VFS_ALLOC_H_

#include <stddef.h>

/* Simplistic arena allocator.
 * To be used untill a 'true' allocator is available.
 */

#define VFS_ALLOCATOR_SIZE 1024 * 1024

void* vfs_malloc(size_t size);
void* vfs_realloc(void* ptr, size_t new_size);

void vfs_free(void* ptr);

#endif
