/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/kernel.c
 *
 ******************************************************************************/

#include "kernel.h"

#include <efi.h>
#include <efidef.h>
#include <efilib.h>

#include "common.h"
#include "exit.h"
#include "log.h"

typedef struct _RISCV_EFI_BOOT_PROTOCOL RISCV_EFI_BOOT_PROTOCOL;
typedef EFI_STATUS(EFIAPI* EFI_GET_BOOT_HARTID)(IN RISCV_EFI_BOOT_PROTOCOL* This, OUT UINTN* BootHartId);
struct _RISCV_EFI_BOOT_PROTOCOL {
	UINT64 Revision;
	EFI_GET_BOOT_HARTID GetBootHartId;
};
static EFI_GUID RISCV_EFI_BOOT_PROTOCOL_GUID = {
    0xccd15fec, 0x6f73, 0x4eec, {0x83, 0x95, 0x3e, 0x69, 0xe4, 0xb9, 0x40, 0xbf}
};

void kernel_start(void) {
	START;
	EFI_STATUS status;

	UINTN map_size = 0;
	EFI_MEMORY_DESCRIPTOR* mem_map = nullptr;
	UINTN map_key;
	UINTN desc_size;
	UINT32 desc_version;

	log(L"Exiting UEFI-boot...");
	exit_boot();

	RISCV_EFI_BOOT_PROTOCOL* riscv_boot_protocol = nullptr;
	UINTN boot_hartid = 0;
	status = g_system_table->BootServices->LocateProtocol(&RISCV_EFI_BOOT_PROTOCOL_GUID, nullptr,
	                                                      (void**)&riscv_boot_protocol);
	if (EFI_ERROR(status) || !riscv_boot_protocol) {
		err(L"Failed to locate RISCV_EFI_BOOT_PROTOCOL: %u", status);
		exit();
	}
	status = riscv_boot_protocol->GetBootHartId(riscv_boot_protocol, &boot_hartid);
	if (EFI_ERROR(status)) {
		err(L"Failed to get boot hartid: %u", status);
		exit();
	}

	log(L"Creating memory map...");
	status = g_system_table->BootServices->GetMemoryMap(&map_size, nullptr, &map_key, &desc_size, &desc_version);
	if (status != EFI_BUFFER_TOO_SMALL) {
		err(L"Failed to get memory map size. BootServices.GetMemoryMap() return code: %u", status);
		exit();
	}

	map_size += 8 * desc_size;

	mem_map = AllocatePool(map_size);
	if (mem_map == nullptr) {
		err(L"Failed to allocate memory map");
		exit();
	}

	status = g_system_table->BootServices->GetMemoryMap(&map_size, mem_map, &map_key, &desc_size, &desc_version);
	if (EFI_ERROR(status)) {
		err(L"Failed to create memory map. BootServices.GetMemoryMap() return code: %u", status);
		exit();
	}

	log(L"Starting BigOS...");
	log(L"FDT address: %lX", g_fdt);
	status = g_system_table->BootServices->ExitBootServices(g_image_handle, map_key);
	if (EFI_ERROR(status)) {
		err(L"Failed to exit boot services. BootServices.ExitBootServices() return code: %u", status);
		exit();
	}

	typedef void (*kernel_entry_t)(UINTN, const void*);
	kernel_entry_t entry = (kernel_entry_t)(g_kernel_app.entry_address);

	__asm__ volatile("mv ra, zero\n\t"
	                 "mv a0, %[boot_hartid]\n\t"
	                 "mv a1, %[g_fdt]\n\t"
	                 "jr %[entry]"
	                 :
	                 : [boot_hartid] "r"(boot_hartid), [g_fdt] "r"(g_fdt), [entry] "r"(entry)
	                 : "memory");

	while (1); // Kernel shouldn't return
}
