#include <unity.h>
#include <stdlib.h>
#include "allocator.h"
#include "memory_management/include/physical_memory/manager.h"
#include "stdbigos/memory_types.h"

#define TEST_PAGES 64
#define TEST_SIZE  (TEST_PAGES * PAGE_SIZE)

#define RESERVED1_OFFSET 0
#define RESERVED1_SIZE   PAGE_SIZE

#define RESERVED2_OFFSET (32 * PAGE_SIZE)
#define RESERVED2_SIZE   (2 * PAGE_SIZE)

#define TOTAL_RESERVED_PAGES 3

typedef struct {
	memory_area_t* areas;
	size_t count;
	size_t index;
} reserved_ctx_t;

static bool enumerator_stub(void* user, memory_area_t* out) {
	reserved_ctx_t* ctx = user;
	if (ctx->index >= ctx->count) {
		ctx->index = 0;
		return false;
	}
	*out = ctx->areas[ctx->index++];
	return true;
}

static void* g_area_buf;
static memory_region_t g_header_region;
static memory_area_t g_area;
static size_t g_header_pages;

void setUp(void) {
	g_area_buf = aligned_alloc(PAGE_SIZE, TEST_SIZE);

	g_area = (memory_area_t){
		.addr = (uintptr_t)g_area_buf,
		.size = TEST_SIZE,
	};

	memory_area_t reserved[] = {
		{.addr = g_area.addr + RESERVED1_OFFSET, .size = RESERVED1_SIZE},
		{.addr = g_area.addr + RESERVED2_OFFSET, .size = RESERVED2_SIZE},
	};
	reserved_ctx_t ctx = {.areas = reserved, .count = 2, .index = 0};

	memory_area_t header_area;
	error_t err = pmallocator_get_header(g_area, enumerator_stub, &ctx, &header_area);
	TEST_ASSERT_EQUAL(ERR_NONE, err);
	TEST_ASSERT_EQUAL(g_area.addr + PAGE_SIZE, header_area.addr);
	TEST_ASSERT_TRUE(header_area.size > 0);
	TEST_ASSERT_EQUAL(0, header_area.size % PAGE_SIZE);
	g_header_pages = header_area.size / PAGE_SIZE;

	g_header_region = (memory_region_t){
		.addr = (void*)header_area.addr,
		.size = header_area.size,
	};

	ctx.index = 0;
	error_t err2 = pmallocator_init_region(g_area, g_header_region, enumerator_stub, &ctx);
	TEST_ASSERT_EQUAL(ERR_NONE, err2);
}

void tearDown(void) {
	free(g_area_buf);
	g_area_buf = NULL;
}


static void test_allocate_single_page(void) {
	memory_area_t result;
	error_t err = pmallocator_allocate(FRAME_ORDER_4KiB, g_header_region, &result);
	TEST_ASSERT_EQUAL(ERR_NONE, err);
	TEST_ASSERT_EQUAL(PAGE_SIZE, result.size);
	TEST_ASSERT_EQUAL(0, result.addr % PAGE_SIZE);
	TEST_ASSERT_TRUE(result.addr >= g_area.addr);
	TEST_ASSERT_TRUE(result.addr + result.size <= g_area.addr + TEST_SIZE);
}

static void test_allocate_until_full(void) {
	size_t expected = TEST_PAGES - TOTAL_RESERVED_PAGES - g_header_pages;
	size_t count = 0;
	memory_area_t result;
	size_t prev_addr = -1;

	while (pmallocator_allocate(FRAME_ORDER_4KiB, g_header_region, &result) == ERR_NONE) {
		TEST_ASSERT_FALSE(
			do_memory_areas_overlap(result,(memory_area_t){ .addr = g_area.addr + RESERVED1_OFFSET, .size = RESERVED1_SIZE })
		);

		TEST_ASSERT_FALSE(
			do_memory_areas_overlap(result,(memory_area_t){ .addr = g_area.addr + RESERVED2_OFFSET, .size = RESERVED2_SIZE })
		);

		TEST_ASSERT_FALSE(
			prev_addr == result.addr
		);

		prev_addr = result.addr;
		count++;
	}
	TEST_ASSERT_EQUAL(expected, count);
}


static void test_free_and_reallocate(void) {
	memory_area_t first;
	memory_area_t second;
	error_t err = pmallocator_allocate(FRAME_ORDER_4KiB, g_header_region, &first);
	TEST_ASSERT_EQUAL(ERR_NONE, err);
	error_t err2 = pmallocator_free(first, g_header_region);
	TEST_ASSERT_EQUAL(ERR_NONE, err2);
	error_t err3 = pmallocator_allocate(FRAME_ORDER_4KiB, g_header_region, &second);
	TEST_ASSERT_EQUAL(ERR_NONE, err3);
	error_t err4 = pmallocator_free(second, g_header_region);
	TEST_ASSERT_EQUAL(ERR_NONE, err4);
	TEST_ASSERT_EQUAL(first.addr, second.addr);
}

static void test_free_double_free(void) {
	memory_area_t result;
	(void)pmallocator_allocate(FRAME_ORDER_4KiB, g_header_region, &result);
	error_t err = pmallocator_free(result, g_header_region);
	TEST_ASSERT_EQUAL(ERR_NONE, err);
	error_t err2 = pmallocator_free(result, g_header_region);
	TEST_ASSERT_EQUAL(ERR_NOT_VALID, err2);
}

static void test_free_out_of_bounds(void) {
	memory_area_t bad = { .addr = g_area.addr - PAGE_SIZE, .size = PAGE_SIZE };
	error_t err = pmallocator_free(bad, g_header_region);
	TEST_ASSERT_EQUAL(ERR_OUT_OF_BOUNDS, err);
}

static void test_free_not_allocated(void) {
	memory_area_t not_allocated = { .addr = g_area.addr + 2 * PAGE_SIZE, .size = PAGE_SIZE };
	error_t err = pmallocator_free(not_allocated, g_header_region);
	TEST_ASSERT_EQUAL(ERR_NOT_VALID, err);
}


static void test_full_interleaved(void) {
	memory_area_t a;
	memory_area_t b;
	memory_area_t c;
	memory_area_t d;
	(void)pmallocator_allocate(FRAME_ORDER_4KiB, g_header_region, &a);
	(void)pmallocator_allocate(FRAME_ORDER_4KiB, g_header_region, &b);
	(void)pmallocator_allocate(FRAME_ORDER_4KiB, g_header_region, &c);
	error_t err = pmallocator_free(b, g_header_region);
	TEST_ASSERT_EQUAL(ERR_NONE, err);
	(void)pmallocator_allocate(FRAME_ORDER_4KiB, g_header_region, &d);
	TEST_ASSERT_EQUAL(b.addr, d.addr);
}

static void test_full_cycle(void) {
	size_t expected = TEST_PAGES - TOTAL_RESERVED_PAGES - g_header_pages;
	memory_area_t results[TEST_PAGES];
	size_t count = 0;

	while (pmallocator_allocate(FRAME_ORDER_4KiB, g_header_region, &results[count]) == ERR_NONE) {
		count++;
	}

	for (size_t i = 0; i < count; i++) {
		error_t err = pmallocator_free(results[i], g_header_region);
		TEST_ASSERT_EQUAL(ERR_NONE, err);
	}

	size_t count2 = 0;
	memory_area_t tmp;
	while (pmallocator_allocate(FRAME_ORDER_4KiB, g_header_region, &tmp) == ERR_NONE) {
		count2++;
	}

	TEST_ASSERT_EQUAL(expected, count);
	TEST_ASSERT_EQUAL(count, count2);
}


int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_allocate_single_page);
	RUN_TEST(test_allocate_until_full);
	RUN_TEST(test_free_and_reallocate);
	RUN_TEST(test_free_double_free);
	RUN_TEST(test_free_out_of_bounds);
	RUN_TEST(test_free_not_allocated);
	RUN_TEST(test_full_interleaved);
	RUN_TEST(test_full_cycle);
	return UNITY_END();
}
