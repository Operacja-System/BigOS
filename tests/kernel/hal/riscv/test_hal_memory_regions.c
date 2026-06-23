#include <libboot/dt/dt.h>
#include <string.h>
#include <unity.h>

#include "hal/include/memory_regions.h"

static bool g_hal_initialized = true;
static int g_fake_dtb;

// ======== iHAL stubs ========
bool ihal_is_init(void) {
	return g_hal_initialized;
}

error_t ihal_get_dtb(void** dtbOUT) {
	if (dtbOUT == NULL)
		return ERR_BAD_ARG;
	*dtbOUT = &g_fake_dtb;
	return ERR_NONE;
}

// ======== DT stubs ========
// Node ids used by this test topology:
// root(1) -> n10(non-memory) -> n20(memory) -> n30(memory)
// reserved-memory root(50) -> n60(reserved child)

enum {
	NODE_ROOT = 1,
	NODE_NON_MEMORY = 10,
	NODE_MEMORY_A = 20,
	NODE_MEMORY_B = 30,
	NODE_RESERVED_ROOT = 50,
	NODE_RESERVED_CHILD = 60,
};

enum {
	PROP_DEVICE_TYPE_MEMORY_A = 100,
	PROP_DEVICE_TYPE_MEMORY_B = 101,
	PROP_REG_MEMORY_A = 200,
	PROP_REG_MEMORY_B = 201,
	PROP_REG_RESERVED_CHILD = 202,
};

static const u8 s_memory_word[] = {'m', 'e', 'm', 'o', 'r', 'y'};

// address_cells=1, size_cells=1 => each entry is 8 bytes (u32 addr + u32 size), big-endian.
static const u8 s_reg_memory_a[] = {
    0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, // [0x1000, 0x2000]
    0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x10, 0x00, // [0x4000, 0x1000]
};

static const u8 s_reg_memory_b[] = {
    0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x10, 0x00, // [0x8000, 0x1000]
};

static const u8 s_reg_reserved_child[] = {
    0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x20, 0x00, // [0xA000, 0x2000]
};

error_t dt_init(const void* fdt, fdt_t* obj) {
	if (fdt == NULL || obj == NULL)
		return ERR_BAD_ARG;

	memset(obj, 0, sizeof(*obj));
	obj->root_node = NODE_ROOT;
	return ERR_NONE;
}

void dt_reset([[maybe_unused]] fdt_t* obj) {}

error_t dt_get_node_in_subtree_by_path(const fdt_t* fdt, dt_node_t node, const char* node_path, dt_node_t* nodeOUT) {
	(void)fdt;
	(void)node;
	(void)node_path;
	(void)nodeOUT;
	return ERR_NOT_FOUND;
}

error_t dt_get_node_by_path(const fdt_t* fdt, const char* node_path, dt_node_t* nodeOUT) {
	(void)fdt;
	if (strcmp(node_path, "/reserved-memory") == 0) {
		*nodeOUT = NODE_RESERVED_ROOT;
		return ERR_NONE;
	}

	return ERR_NOT_FOUND;
}

error_t dt_get_node_child(const fdt_t* fdt, dt_node_t node, dt_node_t* nodeOUT) {
	(void)fdt;
	if (node == NODE_ROOT) {
		*nodeOUT = NODE_NON_MEMORY;
		return ERR_NONE;
	}

	if (node == NODE_RESERVED_ROOT) {
		*nodeOUT = NODE_RESERVED_CHILD;
		return ERR_NONE;
	}

	return ERR_NOT_FOUND;
}

error_t dt_get_node_sibling(const fdt_t* fdt, dt_node_t node, dt_node_t* nodeOUT) {
	(void)fdt;
	if (node == NODE_NON_MEMORY) {
		*nodeOUT = NODE_MEMORY_A;
		return ERR_NONE;
	}

	if (node == NODE_MEMORY_A) {
		*nodeOUT = NODE_MEMORY_B;
		return ERR_NONE;
	}

	return ERR_NOT_FOUND;
}

error_t dt_get_node_name(const fdt_t* fdt, dt_node_t node, buffer_t* bufOUT) {
	(void)fdt;
	(void)node;
	(void)bufOUT;
	return ERR_NOT_FOUND;
}

error_t dt_get_node_name_ptr(const fdt_t* fdt, dt_node_t node, const char** ptrOUT) {
	(void)fdt;
	(void)node;
	(void)ptrOUT;
	return ERR_NOT_FOUND;
}

error_t dt_get_prop_by_name(const fdt_t* fdt, dt_node_t node, const char* prop_name, dt_prop_t* propOUT) {
	(void)fdt;
	if (strcmp(prop_name, "device_type") == 0) {
		if (node == NODE_MEMORY_A) {
			*propOUT = PROP_DEVICE_TYPE_MEMORY_A;
			return ERR_NONE;
		}
		if (node == NODE_MEMORY_B) {
			*propOUT = PROP_DEVICE_TYPE_MEMORY_B;
			return ERR_NONE;
		}
		return ERR_NOT_FOUND;
	}

	if (strcmp(prop_name, "reg") == 0) {
		if (node == NODE_MEMORY_A) {
			*propOUT = PROP_REG_MEMORY_A;
			return ERR_NONE;
		}
		if (node == NODE_MEMORY_B) {
			*propOUT = PROP_REG_MEMORY_B;
			return ERR_NONE;
		}
		if (node == NODE_RESERVED_CHILD) {
			*propOUT = PROP_REG_RESERVED_CHILD;
			return ERR_NONE;
		}
		return ERR_NOT_FOUND;
	}

	return ERR_NOT_FOUND;
}

error_t dt_get_first_prop(const fdt_t* fdt, dt_node_t node, dt_prop_t* propOUT) {
	(void)fdt;
	(void)node;
	(void)propOUT;
	return ERR_NOT_FOUND;
}

error_t dt_get_next_prop(const fdt_t* fdt, dt_prop_t prop, dt_prop_t* propOUT) {
	(void)fdt;
	(void)prop;
	(void)propOUT;
	return ERR_NOT_FOUND;
}

error_t dt_get_prop_name(const fdt_t* fdt, dt_prop_t prop, buffer_t* bufOUT) {
	(void)fdt;
	(void)prop;
	(void)bufOUT;
	return ERR_NOT_FOUND;
}

error_t dt_get_prop_name_ptr(const fdt_t* fdt, dt_prop_t prop, const char** ptrOUT) {
	(void)fdt;
	(void)prop;
	(void)ptrOUT;
	return ERR_NOT_FOUND;
}

error_t dt_get_prop_buffer(const fdt_t* fdt, dt_prop_t prop, buffer_t* bufOUT) {
	(void)fdt;
	switch (prop) {
	case PROP_DEVICE_TYPE_MEMORY_A:
	case PROP_DEVICE_TYPE_MEMORY_B: *bufOUT = make_buffer((void*)s_memory_word, sizeof(s_memory_word)); return ERR_NONE;
	case PROP_REG_MEMORY_A:         *bufOUT = make_buffer((void*)s_reg_memory_a, sizeof(s_reg_memory_a)); return ERR_NONE;
	case PROP_REG_MEMORY_B:         *bufOUT = make_buffer((void*)s_reg_memory_b, sizeof(s_reg_memory_b)); return ERR_NONE;
	case PROP_REG_RESERVED_CHILD:
		*bufOUT = make_buffer((void*)s_reg_reserved_child, sizeof(s_reg_reserved_child));
		return ERR_NONE;
	default: return ERR_NOT_FOUND;
	}
}

error_t dt_get_rsv_mem_entry(const fdt_t* fdt, u32 index, fdt_rsv_entry* entryOUT) {
	(void)fdt;
	if (index == 0) {
		entryOUT->address = 0x9000;
		entryOUT->size = 0x1000;
		return ERR_NONE;
	}

	return ERR_OUT_OF_BOUNDS;
}

error_t dt_get_reg_cell_counts(const fdt_t* fdt, dt_node_t node, u32* address_cellsOUT, u32* size_cellsOUT) {
	(void)fdt;
	if (node == NODE_ROOT || node == NODE_RESERVED_ROOT) {
		*address_cellsOUT = 1;
		*size_cellsOUT = 1;
		return ERR_NONE;
	}

	return ERR_NOT_FOUND;
}

// ======== tests ========
void setUp(void) {
	g_hal_initialized = true;
}

void tearDown(void) {}

static void test_memory_iterator_returns_not_initialized_when_hal_not_ready(void) {
	g_hal_initialized = false;

	hal_memory_iterator_t iter = {0};
	error_t err = hal_get_memory_regions_iterator(&iter);
	TEST_ASSERT_EQUAL(ERR_NOT_INITIALIZED, err);
}

static void test_memory_iterator_enumerates_all_memory_regions(void) {
	hal_memory_iterator_t iter = {0};
	error_t err = hal_get_memory_regions_iterator(&iter);
	TEST_ASSERT_EQUAL(ERR_NONE, err);

	physical_memory_region_t r = {0};

	err = hal_get_next_memory_region(&iter, &r);
	TEST_ASSERT_EQUAL(ERR_NONE, err);
	TEST_ASSERT_EQUAL_HEX64(0x1000, (uintptr_t)r.addr);
	TEST_ASSERT_EQUAL_HEX64(0x2000, r.size);

	err = hal_get_next_memory_region(&iter, &r);
	TEST_ASSERT_EQUAL(ERR_NONE, err);
	TEST_ASSERT_EQUAL_HEX64(0x4000, (uintptr_t)r.addr);
	TEST_ASSERT_EQUAL_HEX64(0x1000, r.size);

	err = hal_get_next_memory_region(&iter, &r);
	TEST_ASSERT_EQUAL(ERR_NONE, err);
	TEST_ASSERT_EQUAL_HEX64(0x8000, (uintptr_t)r.addr);
	TEST_ASSERT_EQUAL_HEX64(0x1000, r.size);

	err = hal_get_next_memory_region(&iter, &r);
	TEST_ASSERT_EQUAL(ERR_NOT_FOUND, err);
}

static void test_reserved_iterator_enumerates_memreserve_then_reserved_memory_node(void) {
	hal_reserved_memory_iterator_t iter = {0};
	error_t err = hal_get_reserved_regions_iterator(&iter);
	TEST_ASSERT_EQUAL(ERR_NONE, err);

	memory_area_t area = {0};

	err = hal_get_next_reserved_region(&iter, &area);
	TEST_ASSERT_EQUAL(ERR_NONE, err);
	TEST_ASSERT_EQUAL_HEX64(0x9000, area.addr);
	TEST_ASSERT_EQUAL_HEX64(0x1000, area.size);

	err = hal_get_next_reserved_region(&iter, &area);
	TEST_ASSERT_EQUAL(ERR_NONE, err);
	TEST_ASSERT_EQUAL_HEX64(0xA000, area.addr);
	TEST_ASSERT_EQUAL_HEX64(0x2000, area.size);

	err = hal_get_next_reserved_region(&iter, &area);
	TEST_ASSERT_EQUAL(ERR_NOT_FOUND, err);
}

static void test_public_api_null_args_return_bad_arg(void) {
	hal_memory_iterator_t mem_iter = {0};
	hal_reserved_memory_iterator_t res_iter = {0};
	physical_memory_region_t pmr = {0};
	memory_area_t area = {0};

	TEST_ASSERT_EQUAL(ERR_BAD_ARG, hal_get_memory_regions_iterator(NULL));
	TEST_ASSERT_EQUAL(ERR_BAD_ARG, hal_get_reserved_regions_iterator(NULL));
	TEST_ASSERT_EQUAL(ERR_BAD_ARG, hal_get_next_memory_region(NULL, &pmr));
	TEST_ASSERT_EQUAL(ERR_BAD_ARG, hal_get_next_memory_region(&mem_iter, NULL));
	TEST_ASSERT_EQUAL(ERR_BAD_ARG, hal_get_next_reserved_region(NULL, &area));
	TEST_ASSERT_EQUAL(ERR_BAD_ARG, hal_get_next_reserved_region(&res_iter, NULL));
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_memory_iterator_returns_not_initialized_when_hal_not_ready);
	RUN_TEST(test_memory_iterator_enumerates_all_memory_regions);
	RUN_TEST(test_reserved_iterator_enumerates_memreserve_then_reserved_memory_node);
	RUN_TEST(test_public_api_null_args_return_bad_arg);
	return UNITY_END();
}
