#ifndef _FILE_TABLE_H_
#define _FILE_TABLE_H_

#include "vfs.h"

#define PIPE_HANDLE (ServiceHandle_t)9391209381209132

FtEntry_t* ft_add_entry(ServiceHandle_t handle, u64 file_id, u64 attributes);

#endif
