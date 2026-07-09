#include "hal/include/address_space.h"
#include "memory_management/include/physical_memory/manager.h"

#define FRAME_SIZES_COUNT 3
static const u32 frame_sizes[FRAME_SIZES_COUNT] = {
    0x1000UL, // 4KiB
    0x200000UL, // 2 MiB
    0x40000000UL, // 1GiB
};

static const frame_order_t frame_orders[FRAME_SIZES_COUNT] = {
    FRAME_ORDER_4KiB,
    FRAME_ORDER_2MiB,
    FRAME_ORDER_1GiB,
};

const u32* hal_get_available_frame_sizes(u32* countOUT) {
    *countOUT = FRAME_SIZES_COUNT;
    return frame_sizes;
}

typedef struct {
    uintptr_t page_table_root;
    hal_address_space_flag_t flags;
} rs39_address_space_t;

#define PTE_VALID 1
#define PTE_PERM_R 2
#define PTE_PERM_W 4
#define PTE_PERM_X 8
#define PTE_USER 16
#define PTE_GLOBAL 32
#define PTE_ACCESS 64
#define PTE_DIRTY 128

static inline u64 as_flags_to_riscv(hal_address_space_flag_t as_flags, hal_address_region_flag_t asr_flags) {
    u64 flags = 0;
    if (as_flags & HAL_AS_GLOBAL) flags |= PTE_GLOBAL;
    if (as_flags & HAL_AS_USER) flags |= PTE_USER;

    if (asr_flags & HAL_ASR_FLAGS_READ) flags |= PTE_PERM_R;
    if (asr_flags & HAL_ASR_FLAGS_WRITE) flags |= PTE_PERM_W;
    if (asr_flags & HAL_ASR_FLAGS_EXECUTE) flags |= PTE_PERM_X;

    return flags;
}

error_t hal_address_space_map(hal_address_space_t as, uintptr_t vaddr, physical_memory_region_t pmem,
                              hal_address_region_flag_t flags) {
    int pt_leaf_level = 0;
    for (; pt_leaf_level < FRAME_SIZES_COUNT; ++pt_leaf_level) {
        if (pmem.size == frame_sizes[pt_leaf_level])
            break;
    }
    if (pt_leaf_level == FRAME_SIZES_COUNT)
        return ERR_BAD_ARG;
    if ((vaddr & (frame_sizes[pt_leaf_level] - 1)) != 0)
        return ERR_BAD_ARG;

    rs39_address_space_t *rs39_as = (rs39_address_space_t*)as.data;
    uintptr_t page_table_root_phys = rs39_as->page_table_root;
    u64 *page_table_root_virt = (u64*)g_physical_to_effective(page_table_root_phys);

    u64 *page = page_table_root_virt;
    size_t page_index;
    u64 next_level_pte;

    physical_memory_region_t mem_reg = { 0 };
    memory_region_t vmem_reg;

    error_t res;

    for (int current_level = FRAME_SIZES_COUNT-1; current_level > pt_leaf_level; --current_level) {
        page_index = (vaddr >> (12 + 9 * current_level)) & 0x1FF;
        next_level_pte = page[page_index];
        if (!(next_level_pte & PTE_VALID)) {
            res = phys_mem_alloc_frame(FRAME_ORDER_4KiB, &mem_reg);
            if (res != ERR_NONE)
                return res;
            page[page_index] = (((u64)mem_reg.addr >> 12) << 10) | PTE_VALID;
            vmem_reg = g_physical_reg_to_effective(mem_reg);
            memset(vmem_reg.addr, 0, 4096);
        }

        mem_reg.addr = (void*)((page[page_index] << 2) & ~0xFFF);
        vmem_reg = g_physical_reg_to_effective(mem_reg);
        page = vmem_reg.addr;
    }

    page_index = (vaddr >> (12 + 9 * pt_leaf_level)) & 0x1FF;
    page[page_index] = (((u64)pmem.addr >> 12) << 10)
        | as_flags_to_riscv(rs39_as->flags, flags) | PTE_VALID;

    return ERR_NONE;
}
