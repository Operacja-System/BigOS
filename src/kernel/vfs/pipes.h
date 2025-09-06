#ifndef _PIPES_H_
#define _PIPES_H_

#include <stdbigos/error.h>
#include <stdbigos/pstring.h>

#include "vfs.h"

#define MAX_SERVERS 8

error_t pipe_create(KernelPipe_t* out);
error_t pipe_read(u64 idx, u32 bytes, u8* out);
error_t pipe_write(u64 idx, u32 bytes, u8* buff);
error_t pipe_remove(u64 idx);

#endif
