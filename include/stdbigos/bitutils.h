#ifndef _STDBIGOS_BITUTILS_H
#define _STDBIGOS_BITUTILS_H

#include <stdbigos/types.h>

u32 read_be32(const void* addr);

u64 read_be64(const void* addr);

u32 align_u32(u32 num, u32 align);

#endif
