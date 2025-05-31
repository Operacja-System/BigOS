#ifndef _VFS_
#define _VFS_

#include <stdbigos/types.h>

// DEBUG for testing CMake configuration
void vfsmain();

#define SERVICE_HANDLE void* // placeholder for now

typedef struct VFS_PATH {
	u16 size;
	char** path;
} VFS_PATH;

#endif
