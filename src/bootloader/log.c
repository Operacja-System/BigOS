/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/log.c
 *
 ******************************************************************************/

#include "log.h"

#include <efi.h>
#include <efidef.h>
#include <efilib.h>
#include <efistdarg.h>

#define LOG_INDENT 4

static UINTN g_procedure_depth = 0;

void prefix(CHAR16 icon) {
	for (UINTN i = 0; i < g_procedure_depth * LOG_INDENT; ++i) Print(L" ");
	Print(L" [%lc] ", icon);
}

void log(const CHAR16* message, ...) {
	va_list args;

	prefix(' ');

	va_start(args, message);
	VPrint(message, args);
	va_end(args);

	Print(L"\n");
}

void warn(const CHAR16* message, ...) {
	va_list args;

	prefix('!');

	va_start(args, message);
	VPrint(message, args);
	va_end(args);

	Print(L"\n");
}

void err(const CHAR16* message, ...) {
	va_list args;

	prefix('X');

	va_start(args, message);
	VPrint(message, args);
	va_end(args);

	Print(L"\n");
}

void log_procedure_start(void) {
	g_procedure_depth++;
}

void log_procedure_end(void) {
	g_procedure_depth--;
}

void log_set_depth(UINTN depth) {
	g_procedure_depth = depth;
}
