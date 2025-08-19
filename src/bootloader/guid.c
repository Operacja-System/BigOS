/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/guid.c
 *
 ******************************************************************************/

#include "guid.h"

INTN guid_compare(EFI_GUID* a, EFI_GUID* b) {
	if (a->Data1 != b->Data1)
		return 0;
	if (a->Data2 != b->Data2)
		return 0;
	if (a->Data3 != b->Data3)
		return 0;
	for (UINTN i = 0; i < 8; ++i) {
		if (a->Data4[i] != b->Data4[i])
			return 0;
	}
	return 1;
}
