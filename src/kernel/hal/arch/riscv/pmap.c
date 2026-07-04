#include "pmap.h"

#include <libcore/error.h>
#include <libcore/memory_types.h>
#include <libcore/string.h>
#include <memory_management/include/physical_memory/manager.h>

#include "hal/include/address_space.h"
#include "pte.h"

error_t pmap_allocate_page(paddr_t* pa, pmap_page_t page_type) {
	physical_memory_region_t phys_region;
	error_t err = phys_mem_alloc_frame(g_pmap_frame_order[page_type], &phys_region);
	if (err == ERR_NONE) {
		memset(physical_to_effective(phys_region.addr), 0, g_pmap_frame_order[page_type]);
		*pa = (paddr_t)phys_region.addr;
	}
	return err;
}

error_t pmap_unallocate_page(paddr_t pa, pmap_page_t page_type) {
	physical_memory_region_t phys_region = {.addr = (__phys void*)pa, .size = g_pmap_frame_order[page_type]};
	error_t err = phys_mem_free_frame(phys_region);
	if (err == ERR_NOT_VALID) {
		/* TODO: Deal with unallocating invalid frame */
		return ERR_NOT_VALID;
	}
	return ERR_NONE;
}

static error_t pmap_free_ptes_l0(pagetable_t* pt_l0) {
	for (int i = 0; i < PTE_COUNT; i++) {
		pte_t pte = pt_l0[i];
		if (!pte_is_valid(pte))
			continue;
		error_t err = pmap_unallocate_page(pte, PMAP_PAGE);
		if (err != ERR_NONE) {
			return err;
		}
		pte = pte_mark_invalid(pte);
		pt_l0[i] = pte;
	}
	return ERR_NONE;
};

static error_t pmap_free_ptes_l1(pagetable_t* pt_l1) {
	error_t err = ERR_NONE;
	for (int i = 0; i < PTE_COUNT; i++) {
		pte_t pte = pt_l1[i];
		if (!pte_is_valid(pte))
			continue;
		if (pte_is_leaf(pte)) {
			err = pmap_unallocate_page(pte_to_pa(pte), PMAP_MEGAPAGE);
			if (err == ERR_NOT_VALID) {
				return ERR_NOT_VALID;
			}
			pt_l1[i] = pte_mark_invalid(pte);
			continue;
		}
		pagetable_t* pt = (pagetable_t*)pa_to_da(pte_to_pa(pte));
		// go one level down
		err = pmap_free_ptes_l0(pt);
		if (err == ERR_NOT_VALID) {
			return ERR_NOT_VALID;
		}
		// actual pte
		err = pmap_unallocate_page(pte_to_pa(pte), PMAP_PAGE);
		if (err == ERR_NOT_VALID) {
			return ERR_NOT_VALID;
		}
		pte = pte_mark_invalid(pte);
		pt_l1[i] = pte;
	}
	return ERR_NONE;
};

static error_t pmap_free_ptes_l2(pagetable_t* pt_l2) {
	error_t err = ERR_NONE;
	for (int i = 0; i < PTE_COUNT; i++) {
		pte_t pte = pt_l2[i];
		if (!pte_is_valid(pte))
			continue;
		if (pte_is_leaf(pte)) {
			err = pmap_unallocate_page(pte_to_pa(pte), PMAP_HUGEPAGE);
			if (err == ERR_NOT_VALID) {
				return ERR_NOT_VALID;
			}
			pt_l2[i] = pte_mark_invalid(pte);
			continue;
		}
		pagetable_t* pt = (pagetable_t*)pa_to_da(pte_to_pa(pte));
		// go level down
		err = pmap_free_ptes_l1(pt);
		if (err != ERR_NONE) {
			return err;
		}
		// actual pte
		err = pmap_unallocate_page(pte_to_pa(pte), PMAP_PAGE);
		if (err == ERR_NOT_VALID) {
			return ERR_NOT_VALID;
		}
		pte = pte_mark_invalid(pte);
		pt_l2[i] = pte;
	}
	return ERR_NONE;
};

error_t pmap_free_pagetable(paddr_t pt) {
    pagetable_t* root_pt = pa_to_da(pt);
	if (root_pt == nullptr)
		return ERR_BAD_ARG;
	error_t err = pmap_free_ptes_l2(root_pt);
	if(err != ERR_NONE)
	    return err;
	err = pmap_unallocate_page(pt, PMAP_PAGE);
	return err;
}

pte_t* pmap_pte_ptr_lookup_valid(pagetable_t pt, vaddr_t va) {
	for (int level = PT_LEVELS - 1; level >= 0; --level) {
		pte_t* ptep = pte_ptr(pt, va_vpn(level, va));
		if (pte_is_valid(*ptep)) {
			if (pte_is_leaf(*ptep)) {
				return ptep;
			}
			pt = pte_to_pa(*ptep);
		} else {
			return nullptr;
		}
	}
	return nullptr;
}

pte_t* pmap_pte_ptr_lookup(pagetable_t pt, vaddr_t va, pmap_page_t page_type) {
	int desired_page_level = (int)page_type;
	for (int level = PT_LEVELS - 1; level > desired_page_level; --level) {
		pte_t* ptep = pte_ptr(pt, va_vpn(level, va));
		if (pte_is_valid(*ptep)) {
			if (pte_is_leaf(*ptep)) {
				if (level == desired_page_level) {
					return ptep;
				}
				return nullptr;
			}
			pt = pte_to_pa(*ptep);
		} else {
		    if(level == desired_page_level) {
				return ptep;
			}
			paddr_t pte_new_pa;
			error_t err = pmap_allocate_page(&pte_new_pa, PMAP_PAGE);
			if(err != ERR_NONE) {
			    /* TODO: Deal with allocation failure */
				return nullptr;
			}
			*ptep = pte_mark_valid(pa_to_pte(pte_new_pa));
			pt = pte_new_pa;
		}
	}
	return pte_ptr(pt, va_vpn(desired_page_level, va));
}
/**
 *
 */
bool pmap_pagefault_fixup(pagetable_t pt, vaddr_t va, hal_address_region_flag_t prot) {
	pte_t* ptep = pmap_pte_ptr_lookup_valid(pt, va);
	if (ptep == nullptr) {
		return false;
	}
	pte_t pte = *ptep;
	bool modified = false;
	switch (prot) {
	case HAL_ASR_FLAGS_READ:
		if ((pte & (PTE_R | PTE_A)) == PTE_R) {
			pte |= PTE_A;
			modified = true;
		}
		break;
	case HAL_ASR_FLAGS_WRITE:
		if (((pte & PTE_W) != 0) && ((pte & PTE_DA) != PTE_DA)) {
			pte |= PTE_A | PTE_D;
			modified = true;
		}
		break;
	case HAL_ASR_FLAGS_EXECUTE:
		if ((pte & (PTE_X | PTE_A)) == PTE_X) {
			pte |= PTE_A;
			modified = true;
		}
	}
	if (modified) {
		*ptep = pte;
	}
	return modified;
}
