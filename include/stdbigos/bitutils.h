#ifndef BITUTILS_H
#define BITUTILS_H

#include <stdbigos/types.h>

// Reads a single 32-bit unsigned integer value from the specified address
static inline u32 read_be32(const void* addr) {
	const u8* bytes = (const u8*)addr;
	return ((u32)bytes[0] << 24) | ((u32)bytes[1] << 16) | ((u32)bytes[2] << 8) | ((u32)bytes[3] << 0);
};

// Reads a single 64-bit unsigned integer value from the specified address
static inline u64 read_be64(const void* addr) {
	const u8* bytes = (const u8*)addr;
	return ((u64)bytes[0] << 56) | ((u64)bytes[1] << 48) | ((u64)bytes[2] << 40) | ((u64)bytes[3] << 32) |
	       ((u64)bytes[4] << 24) | ((u64)bytes[5] << 16) | ((u64)bytes[6] << 8) | ((u64)bytes[7] << 0);
};

// Helper for aligning to 4 bytes
static inline u32 align4(u32 off) {
	return (off + 3) & ~3u;
}

// Helper for aligning to 8 bytes
static inline u32 align8(u32 off) {
	return (off + 7) & ~7u;
}

// Helper for aligning to 16 bytes
static inline u32 align16(u32 off) {
	return (off + 15) & ~15u;
}

// Helper for aligning to 32 bytes
static inline u32 align32(u32 off) {
	return (off + 31) & ~31u;
}

#endif
