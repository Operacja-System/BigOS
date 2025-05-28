#ifndef BITUTILS_H
#define BITUTILS_H

#include <stdbigos/types.h>

u32 read_be32(const void* addr);

u64 read_be64(const void* addr);

u32 alignN(u32 num, u32 align);

#endif
