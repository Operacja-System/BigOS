#ifndef STDBIGOS_BITUTILS
#define STDBIGOS_BITUTILS

#include <stdbigos/types.h>

/// @addtogroup stdbigos
/// @{
/// @addtogroup bitutils
/// @{

u16 read_be16(const void* addr);

u32 read_be32(const void* addr);

u64 read_be64(const void* addr);

u16 read_le16(const void* addr);

u32 read_le32(const void* addr);

u64 read_le64(const void* addr);

u32 align_u32(u32 num, u32 align);

/// @}
/// @}

#endif // !STDBIGOS_BITUTILS
