#include "page_table_entry.h"

#include <stdbigos/string.h>

#include "virtual_memory/mm_common.h"
#include "virtual_memory/pmm.h"

page_table_entry_t create_page_table_entry(u8 N, u8 PBMT, u8 RSW, u8 flags, page_size_t target_page_size) {
	physical_page_number_t ppn = 0;
	error_t err = alloc_frame(target_page_size, &ppn);
	// TODO: handle err
	page_table_entry_t pte = (N & 0x1ull) << 63 | (PBMT & 0x3ull) << 61 | (ppn & 0xfffffffffffull) << 10 |
							 (RSW & 0x3ull) << 8 | (flags & 0xffull) << 0;
	return pte;
}

void read_page_table_entry(page_table_entry_t pte, u8* N, u8* PBMT, physical_page_number_t* PPN, u8* RSW, u8* flags) {
	if(N) *N = pte >> 63;
	if(PBMT) *PBMT = pte >> 61 & 0x3ull;
	if(PPN) *PPN = pte >> 10 & 0xfffffffffffull;
	if(RSW) *RSW = pte >> 8 & 0x3ull;
	if(flags) *flags = pte & 0xffull;
}

u8 create_flags(bool D, bool A, bool G, bool U, bool X, bool W, bool R, bool V) {
	return ((u8)D & 1u) << 7u | ((u8)A & 1u) << 6u | ((u8)G & 1u) << 5u | ((u8)U & 1u) << 4u | ((u8)X & 1u) << 3u |
		   ((u8)W & 1u) << 2u | ((u8)R & 1u) << 1u | ((u8)V & 1u) << 0u;
}

void read_flags(u8 flags, bool* D, bool* A, bool* G, bool* U, bool* X, bool* W, bool* R, bool* V) {
	if(D) *D = (flags >> 7u) & 1u;
	if(A) *A = (flags >> 6u) & 1u;
	if(G) *G = (flags >> 5u) & 1u;
	if(U) *U = (flags >> 4u) & 1u;
	if(X) *X = (flags >> 3u) & 1u;
	if(W) *W = (flags >> 2u) & 1u;
	if(R) *R = (flags >> 1u) & 1u;
	if(V) *V = (flags >> 0u) & 1u;
}

void destroy_page_table_entry_t(page_table_entry_t* pte) {
	physical_page_number_t ppn = 0;
	u8 rsw = 0;
	u8 flags = 0;
	bool pX = false, pW = false, pR = false, pV = false;
	read_page_table_entry(*pte, nullptr, nullptr, &ppn, &rsw, &flags);
	read_flags(flags, nullptr, nullptr, nullptr, nullptr, &pX, &pW, &pR, &pV);
	if(!pV) return;
	if(!pX && !pW && !pR) { // page wasn't a leaf and all of its child pages need to be destroyed fisrt
		page_table_entry_t* pt_node = physical_to_virtual(ppn << 12u); // pt_note is page_table_entry_t[512]
		for(u16 i = 0; i < 512; ++i) destroy_page_table_entry_t(&pt_node[i]);
	} else {
		*pte = 0;
		free_frame(ppn);
	}
}
