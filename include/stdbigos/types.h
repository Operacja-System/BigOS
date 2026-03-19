#ifndef STDBIGOS_TYPES
#define STDBIGOS_TYPES

#include <stddef.h>
#include <stdint.h>

/// @addtogroup stdbigos
/// @{
/// @addtogroup types
/// @{

typedef uint8_t u8;

typedef uint16_t u16;

typedef uint32_t u32;

typedef uint64_t u64;

typedef int8_t i8;

typedef int16_t i16;

typedef int32_t i32;

typedef int64_t i64;

typedef unsigned long reg_t;

typedef signed long ireg_t;
static_assert(sizeof(reg_t) * 8 == __riscv_xlen);

typedef enum {
	ENDIAN_LITTLE = 0,
	ENDIAN_BIG = 1,
} endianness_t;

/// @}
/// @}

#endif // !STDBIGOS_TYPES
