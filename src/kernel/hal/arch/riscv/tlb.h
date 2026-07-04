#ifndef HAL_RISCV_TLB_H
#define HAL_RISCV_TLB_H

/**
 * @brief Invalidates all address-translation cache entries, for all address spaces
 */
void hal_riscv_tlb_invalidate(void) {
    __asm __volatile("sfence.vma" ::: "memory");
}
#endif /* !HAL_RISCV_TLB_H */
