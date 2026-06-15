#ifndef HAL_RISCV_PMAP_H
#define HAL_RISCV_PMAP_H

#include <address_space/address_space.h>
#include <libcore/error.h>
#include <libcore/types.h>
#include <memory_management/include/physical_memory/manager.h>
#include <stdint.h>

#include "pte.h"

/**
 * @brief Physical page types supported in SV39 mode.
 * Underlying value specifies which level Pagetable contains it's
 * Pagetable Entry.
 * */
typedef enum : u8 {
	PMAP_PAGE = 0,
	PMAP_MEGAPAGE = 1,
	PMAP_HUGEPAGE = 2,
} pmap_page_t;

/**
 * @brief Correspondence between RISC V physical page and physical frame order
 */
static const frame_order_t g_pmap_frame_order[] = {
    [PMAP_PAGE] = FRAME_ORDER_4KiB, [PMAP_HUGEPAGE] = FRAME_ORDER_2MiB, [PMAP_MEGAPAGE] = FRAME_ORDER_1GiB};

#define SATP_MODE_SV39 (8UL << 60)
#define SATP_MODE      SATP_MODE_SV39
#define SATP_ASID_S    44
#define SATP_ASID_MASK 0x3FUL
#define PTE_COUNT      512 /* Number of PTEs in a pagetable */
#define PT_LEVELS      3   /* Number of pagetable indirection levels */

static inline reg_t make_satp(paddr_t pa, u64 asid) {
	return SATP_MODE | (asid << SATP_ASID_S) | (pa >> PPN_OFFSET);
}

static inline paddr_t satp_to_pa(reg_t satp) {
	return (satp & PPN_MASK) << PPN_OFFSET;
}

static inline u64 satp_to_asid(reg_t satp) {
	return (satp >> SATP_ASID_S) & SATP_ASID_MASK;
}

/**
 * @brief Translates between physical address and it's mapping in kernel
 * address space (direct address).
 */
static inline void* pa_to_da(paddr_t pa) {
	return physical_to_effective((__phys void*)pa);
}

/**
 * @brief Extract pointer to a PTE from the pagetable.
 *
 * @param pt Pagetable physical address
 * @param n Physical Page Number
 * @returns Pointer to the n-th PTE in the pagetable.
 */
static inline pte_t* pte_ptr(pagetable_t pt, u64 n) {
	pagetable_t* ptp = (pagetable_t*)pa_to_da(pt);
	ptp = ptp + n;
	return (pte_t*)ptp;
}

/**
 * @brief Fixes pagefault caused by memory accesses to a page
 * without appropriate Accessed and/or Dirty bits set.
 */
bool pmap_pagefault_fixup(pagetable_t, vaddr_t, hal_address_region_flag_t);

/**
 * @brief Find valid PTE corresponding to the virtual address.
 */
pte_t* pmap_pte_ptr_lookup_valid(pagetable_t, vaddr_t);

/**
 * @brief Find PTE corresponding to the virtual address and page type.
 * Allocates intermediate Pagetable pages if necessary.
 *
 * @return Pointer to the PTE or nullptr if either allocation proved
 * impossible or there already exists a valid PTE correspoding to virtual
 * address, but it's type is different than expected.
 *
 * @note Does not allocate Leaf PTE and associated page frame,
 * to allow mapping of already existing resource.
 */
pte_t* pmap_pte_ptr_lookup(pagetable_t, vaddr_t, pmap_page_t);
/**
 * @brief Unallocate all pages associated with the pagetable.
 *
 */
error_t pmap_free_pagetable(paddr_t);

/**
 * @brief Allocate physical page
 * @param[out] pa, physical address of the allocated page.
 * @param[in] page_type, page_type .
 * @return ERR_NOT_VALID, if allocation of physical memory fails
 *         ERR_NONE otherwise.
 * @todo Physical page bookkeeping.
 */
error_t pmap_allocate_page(paddr_t* pa, pmap_page_t page_type);

/**
 * @brief Unallocate physical page
 * @param[in] pa, physical address of page to be unallocated.
 * @param[in] page_type, page_type specifies size of the allocation.
 * @return ERR_NOT_VALID, if unallocation of physical memory fails
 *         ERR_NONE otherwise.
 * @todo Physical page bookkeeping.
 */
error_t pmap_unallocate_page(paddr_t pa, pmap_page_t page_type);
#endif /* !HAL_RISCV_PMAP_H */
