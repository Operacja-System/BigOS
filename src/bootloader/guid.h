/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/guid.h
 *  Description:	For some reason CompareGuid function doesn't work.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_GUID
#define BIGOS_BOOTLOADER_GUID

#include <efi.h>

INTN guid_compare(EFI_GUID* a, EFI_GUID* b);

#endif // !BIGOS_BOOTLOADER_GUID
