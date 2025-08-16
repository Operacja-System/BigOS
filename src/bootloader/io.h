/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/io.h
 *  Description:	File handling module.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_IO
#define BIGOS_BOOTLOADER_IO

#include <efi.h>
#include <efilib.h>

#include "error.h"

/**
 * @brief	Read from file
 *
 * @param	file - Open file to read from
 * @param	offset - Offset in bytes from the beginning of the file
 * @param	size - Number of bytes to read
 * @param	buffer - Buffer to read to
 *
 * @note	Buffer is expected to be at least size bytes big
 */
[[nodiscard]] status_t read_file(EFI_FILE_PROTOCOL* file, UINTN offset, UINTN size, void* buffer);

/**
 * @brief	Create a buffer with file info
 *
 * @param	file - Open file to get the info of
 * @param	file_info - Returned buffer with file info
 *
 * @note	It's the callers responsibility to FreePool() the buffer
 */
[[nodiscard]] status_t get_file_info(EFI_FILE_PROTOCOL* file, EFI_FILE_INFO** file_info);

/**
 * @brief	Create a buffer with a single directory entry
 *
 * @param	file - File that is expected to be of directory type
 * @param	size - Returned buffer size
 * @param	buffer - Returned buffer with the entry
 *
 * @note	It's the callers responsibility to FreePool() the buffer
 */
[[nodiscard]] status_t get_directory_entry(EFI_FILE_PROTOCOL* file, UINTN* size, void** buffer);

#endif // !BIGOS_BOOTLOADER_IO
