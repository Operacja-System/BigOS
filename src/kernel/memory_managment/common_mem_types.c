#include "common_mem_types.h"

#include <stdbigos/math.h>

memory_area_t memory_region_get_area(memory_region_t reg) {
	const memory_area_t out = {
	    .addr = (uintptr_t)reg.addr,
	    .size = reg.size,
	};
	return out;
}

bool do_memory_areas_overlap(memory_area_t reg1, memory_area_t reg2) {
	bool cond1 = reg1.addr < reg2.addr + reg2.size;
	bool cond2 = reg2.addr < reg1.addr + reg1.size;
	return cond1 && cond2;
}

memory_area_t memory_area_expand_to_alignment(memory_area_t reg, u64 align) {
	memory_area_t aligned_reg = {
	    .addr = ALIGN_DOWN(reg.addr, align),
	    .size = reg.size,
	};
	aligned_reg.size = reg.size + reg.addr - aligned_reg.addr;
	aligned_reg.size = ALIGN_UP(aligned_reg.size, align);
	return aligned_reg;
}

memory_area_t memory_area_shrink_to_alignment(memory_area_t reg, u64 align) {
	memory_area_t aligned_reg = {
	    .addr = ALIGN_UP(reg.addr, align),
	    .size = reg.size,
	};
	aligned_reg.size = reg.size - reg.addr + aligned_reg.addr;
	aligned_reg.size = ALIGN_DOWN(aligned_reg.size, align);
	return aligned_reg;
}

memory_region_t memory_region_shrink_to_alignment(memory_region_t reg, u64 align) {
	memory_area_t area = memory_region_get_area(reg);
	area = memory_area_shrink_to_alignment(area, align);
	const memory_region_t out = {
	    .addr = (void*)area.addr,
	    .size = area.size,
	};
	return out;
}

memory_area_t memory_area_expand_to_alignment_pow2(memory_area_t reg, u64 pow2) {
	memory_area_t aligned_reg = {
	    .addr = ALIGN_DOWN_POW2(reg.addr, pow2),
	    .size = reg.size,
	};
	aligned_reg.size = reg.size + reg.addr - aligned_reg.addr;
	aligned_reg.size = ALIGN_UP_POW2(aligned_reg.size, pow2);
	return aligned_reg;
}

memory_area_t memory_area_shrink_to_alignment_pow2(memory_area_t reg, u64 pow2) {
	memory_area_t aligned_reg = {
	    .addr = ALIGN_UP_POW2(reg.addr, pow2),
	    .size = reg.size,
	};
	aligned_reg.size = reg.size - reg.addr + aligned_reg.addr;
	aligned_reg.size = ALIGN_DOWN_POW2(aligned_reg.size, pow2);
	return aligned_reg;
}

memory_region_t memory_region_shrink_to_alignment_pow2(memory_region_t reg, u64 pow2) {
	memory_area_t area = memory_region_get_area(reg);
	area = memory_area_shrink_to_alignment_pow2(area, pow2);
	const memory_region_t out = {
	    .addr = (void*)area.addr,
	    .size = area.size,
	};
	return out;
}
