#include "stdbigos/version.h"

version_t version_make(u16 generation, u16 feature, u16 patch) {
	return (version_t){.generation = generation, .feature = feature, .patch = patch};
}

version_packed_t version_pack(version_t v) {
	version_packed_t generation_packed = ((version_packed_t)v.generation) << 32;
	version_packed_t feature_packed = ((version_packed_t)v.feature) << 16;
	version_packed_t patch_packed = ((version_packed_t)v.patch);
	return generation_packed | feature_packed | patch_packed;
}

version_t version_unpack(version_packed_t v) {
	u16 generation = v >> 32;
	u16 feature = (v >> 16);
	u16 patch = v;
	return (version_t){.generation = generation, .feature = feature, .patch = patch};
}

bool version_is_compatible(version_t current, version_t required) {
	return current.generation == required.generation && current.feature >= required.feature;
}

bool version_packed_is_compatible(version_packed_t current, version_packed_t required) {
	version_t currentu = version_unpack(current);
	version_t requiredu = version_unpack(required);
	return currentu.generation == requiredu.generation && currentu.feature >= requiredu.feature;
}
