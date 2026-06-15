#ifndef HAL_RISCV_PAGEFAULT
#define HAL_RISCV_PAGEFAULT

#include <address_space/address_space.h>
#include <stddef.h>
#include <stdint.h>

#include "hal/arch/riscv/pmap.h"
#include "hal/arch/riscv/pte.h"
#include "hal/include/address_space.h"
#include "libcore/error.h"

/// Mock process address space
extern address_space_t as;
/**
 * @brief Mock pagefault resolution in virtual address space map.
 */
error_t vm_handle_pagefault(uintptr_t va, paddr_t pt, hal_address_region_flag_t fault_prot, [[maybe_unused]] bool usermode) {
    for(size_t i = 0; i < as.regions_count; ++i) {
        address_space_region_t as_region = as.regions[i];
        if(va >= (uintptr_t)as_region.addr  && va < ((uintptr_t)as_region.addr + as_region.size)) {
            if((as_region.flags & fault_prot) != 0) {
                pte_t* ptep = pmap_pte_ptr_lookup(va, pt, PMAP_PAGE);
                pte_t pte_flags = as_flags_to_pte((as_region.flags));

                paddr_t pa;
                error_t err = pmap_allocate_page(&pa, PMAP_PAGE);
                if(err != ERR_NONE)
                    return err;
                *ptep = pa_to_pte(pa) | pte_flags;
                return ERR_NONE;
            }
            //Violated permission
            return ERR_NOT_VALID;
        }
    }
    return ERR_OUT_OF_BOUNDS;
}
#endif /*!HAL_RISCV_PAGEFAULT */
