#ifndef LIBCORE_BITUTILS
#define LIBCORE_BITUTILS

#include <libcore/types.h>

/// @addtogroup libcore
/// @{
/// @addtogroup bitutils
/// @{

u32 read_be32(const void* addr);

u64 read_be64(const void* addr);

u32 read_le32(const void* addr);

u64 read_le64(const void* addr);

void write_be32(const void* addr, u32 val);

u32 align_u32(u32 num, u32 align);

/// @}
/// @}

#endif // !LIBCORE_BITUTILS
