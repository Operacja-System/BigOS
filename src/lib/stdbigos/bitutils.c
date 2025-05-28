#include <stdbigos/bitutils.h>

u32 read_be32(const void* addr) {
	const u8* bytes = (const u8*)addr;
	return ((u32)bytes[0] << 24) | ((u32)bytes[1] << 16) | ((u32)bytes[2] << 8) | ((u32)bytes[3] << 0);
};

u64 read_be64(const void* addr) {
	const u8* bytes = (const u8*)addr;
	return ((u64)bytes[0] << 56) | ((u64)bytes[1] << 48) | ((u64)bytes[2] << 40) | ((u64)bytes[3] << 32) |
	       ((u64)bytes[4] << 24) | ((u64)bytes[5] << 16) | ((u64)bytes[6] << 8) | ((u64)bytes[7] << 0);
};

u32 alignN(u32 num, u32 align) {
	return (num + (align - 1)) & ~(align - 1);
}
