#ifndef BIGOS_BOOTLOADER_DT_BOOTUTILS
#define BIGOS_BOOTLOADER_DT_BOOTUTILS
// Define this way to keep the style of the bootloader

#include <error.h>

/**
 * @brief	Read FDT from EFI system table and store it in g_fdt
 */
[[nodiscard]] status_t get_FDT(void);

#endif // !BIGOS_BOOTLOADER_DT_BOOTUTILS
