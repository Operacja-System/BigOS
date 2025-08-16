/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/log.h
 *  Description:	Module for logging within the bootloader.
 *
 *  Note:
 *  Use START; at the start of a function to denote the start of a subtask.
 *  When exiting such function use END; or RETURN().
 *  Subtasks are indented accordingly in logs to create tree-like structure.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_LOG
#define BIGOS_BOOTLOADER_LOG

#include <efi.h>
#include <efidef.h>

// Indent log messages coming after this line
#define START log_procedure_start()

// Stop indenting log messages coming after this line
#define END log_procedure_end()

// Stop indenting log messages coming after this line and return
#define RETURN(x)            \
	do {                     \
		log_procedure_end(); \
		return (x);          \
	} while (0)

/**
 * @brief	Logging function
 *
 * @param	message - Standard format string followed by optional to be formatted
 */
void log(const CHAR16* message, ...);

/**
 * @brief	Warning logging function
 *
 * @param	message - Standard format string followed by optional to be formatted
 */
void warn(const CHAR16* message, ...);

/**
 * @brief	Error logging function
 *
 * @param	message - Standard format string followed by optional to be formatted
 */
void err(const CHAR16* message, ...);

/**
 * @brief	Indent log messages coming after this line
 */
void log_procedure_start(void);

/**
 * @brief	Stop indenting log messages coming after this line
 */
void log_procedure_end(void);

/**
 * @brief	Set indentation manually
 *
 * @param	depth - Depth of subsequent log messages.
 *			0 - main level (highest level messages)
 */
void log_set_depth(UINTN depth);

#endif // !BIGOS_BOOTLOADER_LOG
