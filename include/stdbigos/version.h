#ifndef STDBIGOS_VERSION
#define STDBIGOS_VERSION

/// @addtogroup stdbigos
/// @{
/// @addtogroup stdversion
/// @{

#include "stdbigos/types.h"

typedef struct {
	u16 generation;
	u16 feature;
	u16 patch;
} version_t;

typedef u64 version_packed_t;

version_t version_make(u16 generation, u16 feature, u16 patch);

version_packed_t version_pack(version_t v);
version_t version_unpack(version_packed_t v);

bool version_is_compatible(version_t current, version_t required);
bool version_packed_is_compatible(version_packed_t current, version_packed_t required);

/// @}
/// @}

#endif // !STDBIGOS_VERSION
