/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/exit.c
 *
 ******************************************************************************/

#include "exit.h"

#include <efi.h>
#include <efierr.h>
#include <efilib.h>

#include "log.h"

typedef void (*exit_procedure_t)(void);

static UINTN exit_procedure_count = 0;
static UINTN exit_procedure_buffer_size = 0;
static exit_procedure_t* exit_procedures;

#define BUFFER_CHUNK_SIZE 64

static void exit_procedures_call(void) {
	log(L"Calling exit procedures...");
	for (INTN i = exit_procedure_count - 1; i >= 0; --i) {
		exit_procedures[i]();
	}
	FreePool(exit_procedures);
}

void exit(void) {
	log_set_depth(0);
	exit_procedures_call();
	log(L"Exiting UEFI-boot...");
	Exit(EFI_LOAD_ERROR, 0, nullptr);
	// Should not exit
	while (true);
}

void exit_boot(void) {
	exit_procedures_call();
}

void exit_procedure_register(exit_procedure_t function) {
	START;
	if (exit_procedure_buffer_size == 0) {
		exit_procedures = AllocatePool(sizeof(exit_procedure_t) * BUFFER_CHUNK_SIZE);
		if (exit_procedures == nullptr) {
			err(L"Failed to register cleanup function");
			END;
			return;
		}
		exit_procedure_buffer_size = BUFFER_CHUNK_SIZE;
	}

	exit_procedure_count++;

	if (exit_procedure_count > exit_procedure_buffer_size) {
		ReallocatePool(exit_procedures, exit_procedure_buffer_size, exit_procedure_buffer_size + BUFFER_CHUNK_SIZE);
		if (exit_procedures == nullptr) {
			err(L"Failed to register cleanup function");
			END;
			return;
		}
		exit_procedure_buffer_size += BUFFER_CHUNK_SIZE;
	}

	exit_procedures[exit_procedure_count - 1] = function;
	END;
}
