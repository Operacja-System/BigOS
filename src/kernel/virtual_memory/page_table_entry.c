#include "page_table.h"

#include <stdbigos/string.h>

#include "virtual_memory/mm_common.h"
#include "virtual_memory/pmm.h"

page_table_entry_t create_page_table_entry(u8 N, u8 PBMT, u8 RSW, u8 permissions, page_size_t target_page_size) {
	physical_page_number_t ppn = 0;
	error_t err = alloc_frame(target_page_size, &ppn);
	// TODO: handle err
	page_table_entry_t pte = (N & 0x1ull) << 63 | (PBMT & 0x3ull) << 61 | (ppn & 0xfffffffffffull) << 10 |
							 (RSW & 0x3ull) << 8 | (permissions & 0xffull) << 0;
	return pte;
}

void read_page_table_entry(page_table_entry_t pte, u8* N, u8* PBMT, physical_page_number_t* PPN, u8* RSW,
						   u8* permissions) {
	if(N) *N = pte >> 63;
	if(PBMT) *PBMT = pte >> 61 & 0x3ull;
	if(PPN) *PPN = pte >> 10 & 0xfffffffffffull;
	if(RSW) *RSW = pte >> 8 & 0x3ull;
	if(permissions) *permissions = pte & 0xffull;
}

u8 create_permissions(bool D, bool A, bool G, bool U, bool X, bool W, bool R, bool V) {
	return ((u8)D & 1u) << 7u | ((u8)A & 1u) << 6u | ((u8)G & 1u) << 5u | ((u8)U & 1u) << 4u | ((u8)X & 1u) << 3u |
		   ((u8)W & 1u) << 2u | ((u8)R & 1u) << 1u | ((u8)V & 1u) << 0u;
}

void read_permissions(u8 permissions, bool* D, bool* A, bool* G, bool* U, bool* X, bool* W, bool* R, bool* V) {
	if(D) *D = (permissions >> 7u) & 1u;
	if(A) *A = (permissions >> 6u) & 1u;
	if(G) *G = (permissions >> 5u) & 1u;
	if(U) *U = (permissions >> 4u) & 1u;
	if(X) *X = (permissions >> 3u) & 1u;
	if(W) *W = (permissions >> 2u) & 1u;
	if(R) *R = (permissions >> 1u) & 1u;
	if(V) *V = (permissions >> 0u) & 1u;
}

void destroy_page_table_entry_t(page_table_entry_t* pte) {}

error_t add_page_table_entry(page_table_entry_t* root_pte, page_size_t page_size, virtual_page_number_t vpn,
							 page_table_entry_t pte) {}

error_t get_page_table_entry(page_table_entry_t* root_pte, virtual_page_number_t vpn, page_table_entry_t** pteOUT) {}

error_t remove_page_table_entry(page_table_entry_t* root_pte, virtual_page_number_t vpn) {}

#endif //!_KERNEL_VIRTUAL_MEMORY_PAGE_TABLE_H
