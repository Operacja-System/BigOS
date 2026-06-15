/**
 * @file pte.h
 * @brief Internal RISC-V Pagetable macros and functions
 *
 * This header is internal to HAL and should not be included directly by user code.
 */

#ifndef HAL_ARCH_RISCV_PAGE_TABLE_H
#define HAL_ARCH_RISCV_PAGE_TABLE_H

#include <libcore/types.h>
#include <address_space/address_space.h>
#include "hal/include/address_space.h"

typedef u64 vaddr_t;
typedef u64 paddr_t;
typedef u64 pte_t;
typedef pte_t pagetable_t;

/* MODE bits; 4 MSB in 64bit register satp */
#define SATP_SV39  (8UL << 60)
#define PPN_MASK   0xF'FF'FF'FF'FF'FFull /* 44 low bits */
#define PPN_OFFSET 12

#define VA_VPN_WIDTH  9
#define VA_VPN_OFFSET 12
#define VA_VPN_MASK   0x1'FFull
#define VA_VPN0_S     12
#define VA_VPN1_S     21
#define VA_VPN2_S     30

#define PA_PPN_OFFSET 12
#define PA_PPN0_S     12
#define PA_PPN1_S     21
#define PA_PPN2_S     30

#define PTE_PPN_OFFSET 10
#define PTE_PPN0_S     10
#define PTE_PPN1_S     19
#define PTE_PPN2_S     28

/*  */
#define PTE_RSW (3ull << 8) /* RSW */
#define PTE_D   (1ull << 7) /* Dirty */
#define PTE_A   (1ull << 6) /* Accessed */
#define PTE_G   (1ull << 5) /* Global */
#define PTE_U   (1ull << 4) /* User */
#define PTE_X   (1ull << 3) /* Execute */
#define PTE_W   (1ull << 2) /* Write */
#define PTE_R   (1ull << 1) /* Read */
#define PTE_V   (1ull << 0) /* Valid */
#define PTE_RWX (PTE_R | PTE_W | PTE_X)
#define PTE_RW  (PTE_R | PTE_W)
#define PTE_RX  (PTE_R | PTE_X)
#define PTE_WX  (PTE_W | PTE_X)
#define PTE_DA  (PTE_D | PTE_A)

static inline bool pte_is_valid(pte_t pte) {
	return ((pte & PTE_V) != 0);
}

static inline pte_t pte_mark_valid(pte_t pte) {
    return (pte | PTE_V);
}

static inline pte_t pte_mark_invalid(pte_t pte) {
	return (pte & ~PTE_V);
}

static inline paddr_t pte_to_pa(pte_t pte) {
	return (((pte >> PTE_PPN_OFFSET) & PPN_MASK) << PA_PPN_OFFSET);
}

static inline pte_t pa_to_pte(paddr_t pa) {
	return (((pa >> PA_PPN_OFFSET) & PPN_MASK) << PTE_PPN_OFFSET);
}

static inline bool pte_is_leaf(pte_t pte) {
	return ((pte & PTE_R) || ((pte & PTE_WX) == PTE_X));
}

static inline bool pte_is_branch(pte_t pte) {
   return ((pte & PTE_RWX) == 0);
}

static inline pte_t pte_make_branch(paddr_t pa) {
   return pa_to_pte(pa) | PTE_V;
}

static inline u64 va_vpn2(vaddr_t va) {
	return ((va >> VA_VPN2_S) & VA_VPN_MASK);
}

static inline u64 va_vpn1(vaddr_t va) {
	return ((va >> VA_VPN1_S) & VA_VPN_MASK);
}

static inline u64 va_vpn0(vaddr_t va) {
	return ((va >> VA_VPN0_S) & VA_VPN_MASK);
}

static inline u64 va_vpn(uint8_t level, vaddr_t va) {
	switch (level) {
	case 0:  return va_vpn0(va);
	case 1:  return va_vpn1(va);
	case 2:
	default: return va_vpn2(va);
	}
}

static inline hal_address_region_flag_t pte_to_asr_flags(pte_t pte) {
   hal_address_region_flag_t asr_flags = {0};
   if(pte & PTE_R)
       asr_flags |= HAL_ASR_FLAGS_READ;
   if(pte & PTE_W)
       asr_flags |= HAL_ASR_FLAGS_WRITE;
   if(pte & PTE_X)
       asr_flags |= HAL_ASR_FLAGS_EXECUTE;
   return asr_flags;
}

static inline hal_address_space_flag_t pte_to_as_flags(pte_t pte) {
    hal_address_space_flag_t as_flags = {0};
    if(pte & PTE_G)
        as_flags |= HAL_AS_GLOBAL;
    if(pte & PTE_U)
        as_flags |= HAL_AS_USER;
    return as_flags;
}

static inline u64 as_flags_to_pte(address_space_region_flags_t as_flags) {
    u64 flags = 0;
    if (as_flags & HAL_AS_GLOBAL) flags |= PTE_G;
    if (as_flags & HAL_AS_USER) flags |= PTE_U;

    if (as_flags & HAL_ASR_FLAGS_READ) flags |= PTE_R;
    if (as_flags & HAL_ASR_FLAGS_WRITE) flags |= PTE_W;
    if (as_flags & HAL_ASR_FLAGS_EXECUTE) flags |= PTE_X;

    return flags;
}

static inline u64 hal_as_flags_to_pte(hal_address_space_flag_t as_flags, hal_address_region_flag_t asr_flags) {
    u64 flags = 0;
    if (as_flags & HAL_AS_GLOBAL) flags |= PTE_G;
    if (as_flags & HAL_AS_USER) flags |= PTE_U;

    if (asr_flags & HAL_ASR_FLAGS_READ) flags |= PTE_R;
    if (asr_flags & HAL_ASR_FLAGS_WRITE) flags |= PTE_W;
    if (asr_flags & HAL_ASR_FLAGS_EXECUTE) flags |= PTE_X;

    return flags;
}

#endif // !HAL_ARCH_RISCV_PAGE_TABLE_H
