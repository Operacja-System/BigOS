#ifndef _KERNEL_VIRTUAL_MEMORY_PAGE_TABLE_ENTRY_H
#define _KERNEL_VIRTUAL_MEMORY_PAGE_TABLE_ENTRY_H

#include <stdbigos/error.h>
#include <stdbigos/types.h>

#include "mm_common.h"

typedef enum : u8 {
	PTEP_R = (1u << 0u),
	PTEP_W = (1u << 1u),
	PTEP_X = (1u << 2u),
	PTEP_U = (1u << 3u),
	PTEP_G = (1u << 4u),
	PTEP_S = (1u << 5u),
	PTEP_NOT_YET_DEFINED = (1u << 6u),
} page_table_entry_perms_t;
/* (* is an OS reserved bit not yet defines)
 * page_talbe_entry_perms_t structure:
 * u8[0*SGUXWR]
 * perms structure required by RISC-V ISA:
 * u10[*SDAGUXWRV] (D, A and V bits will be always initialized to 0, 0 and 1 so there's no need to specify them)
 */

typedef u64 page_table_entry_t;
/* (03-04-2025)
 * page table entry:
 * |N|PBMT|Reserved|PPN|RSW|D|A|G|U|X|W|R|V|
 * |1| 2  |    7   | 44| 2 |1|1|1|1|1|1|1|1|
 * where:
 * N - reserved by Svnapot extension (must be 0 otherwise)
 * PBMT - reserved by Svpbmt extension (must be 0 otherwise)
 * Reserved - reserved for future standard use (must be 0)
 * PPN - is the phisical page number, it is split into 3(Sv39), 4(Sv48) or 5(Sv57) levels (PPN[0] - PPN[4])
 * RSW - reserved for use by supervisor software (OS)
 * D - dirty bit (has this page been writted to?)
 * A - accessed bit (has this page been accessed?)
 * G - global bit (is this page accessable from multiple address spaces?)
 * U - user bit (can this page be accessed in U-mode?)
 * X - execute permission bit (can data from this page be executed?)
 * W - write permission bit (can data be written to this page?)
 * R - read permission bit (can data be read from this page?)
 * V - valid bit (is this page table entry valid?)
 * further information is avalible here:
 * https://drive.google.com/file/d/17GeetSnT5wW3xNuAHI95-SI1gPGd5sJ_/view
 */

[[nodiscard]] page_table_entry_t create_page_table_entry(u8 N, u8 PBMT, u8 RSW, u8 permissions,
														 page_size_t target_page_size);

void destroy_page_table_entry_t(page_table_entry_t* pte);

void read_page_table_entry(page_table_entry_t pte, u8* N, u8* PBMT, physical_page_number_t* PPN, u8* RSW,
						   u8* permissions);

[[nodiscard]] u8 create_flags(bool D, bool A, bool G, bool U, bool X, bool W, bool R, bool V);

// NOTE: any argument being nullptr is valid and will be ignored
void read_flags(u8 flags, bool* D, bool* A, bool* G, bool* U, bool* X, bool* W, bool* R, bool* V);

[[nodiscard]] error_t add_page_table_entry(page_table_entry_t* root_pte, page_size_t page_size,
										   virtual_page_number_t vpn, page_table_entry_t pte);
[[nodiscard]] error_t get_page_table_entry(page_table_entry_t* root_pte, virtual_page_number_t vpn,
										   page_table_entry_t** pteOUT);
[[nodiscard]] error_t remove_page_table_entry(page_table_entry_t* root_pte, virtual_page_number_t vpn);

#endif //!_KERNEL_VIRTUAL_MEMORY_PAGE_TABLE_ENTRY_H
