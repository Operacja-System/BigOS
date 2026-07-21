/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/error.h
 *  Description:	Bootloader error type.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_ERROR
#define BIGOS_BOOTLOADER_ERROR

typedef enum {
	BOOT_SUCCESS = 1,
	BOOT_ERROR = 0
} status_t;

#endif // !BIGOS_BOOTLOADER_ERROR
