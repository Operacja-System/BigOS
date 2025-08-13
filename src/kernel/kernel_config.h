#ifndef BIGOS_KERNEL_KERNEL_CONFIG
#define BIGOS_KERNEL_KERNEL_CONFIG

#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

#include "memory_managment/physical_memory_manager.h"

typedef enum : u8 {
	KC_MODE_32 = 32,
	KC_MODE_64 = 64,
} kerconf_mode_t;

typedef enum : u8 { KC_MACHINE_RISCV = 1,
} kerconf_machine_t;

typedef enum : u8 {
	KC_CPU_ENDIAN_LITTLE_ENDIAN = 1,
	KC_CPU_ENDIAN_BIG_ENDIAN = 2,
} kerconf_cpu_endian_t;

typedef enum : u8 {
	KC_VMS_BARE = 0,
	KC_VMS_SV39 = 8,
	KC_VMS_SV48 = 9,
	KC_VMS_SV57 = 10,
} kerconf_virtual_memory_scheme_t;

typedef struct {
	kerconf_mode_t mode;
	kerconf_machine_t machine;
	kerconf_virtual_memory_scheme_t active_vms;
	kerconf_virtual_memory_scheme_t target_vms;
	kerconf_cpu_endian_t cpu_endian;
	phys_addr_t device_tree_phys_addr;
} kernel_config_t;

[[nodiscard]] error_t kernel_config_set(kernel_config_t cfg);

typedef enum {
	KERCFG_MODE,               // u8
	KERCFG_MACHINE,            // u8
	KERCFG_ACTIVE_VMS,         // u8
	KERCFG_TARGET_VMS,         // u8
	KERCFG_ADDRESS_SPACE_BITS, // u8
	KERCFG_PT_HEIGHT,          // u8
	KERCFG_CPU_ENDIAN,         // u8
} kercfg_field_t;

[[nodiscard]] buffer_t kernel_config_get(kercfg_field_t field);
[[nodiscard]] buffer_t kernel_config_read_device_tree(const char* node_path, const char* arg_name);

void kernel_config_log();

#endif // !BIGOS_KERNEL_KERNEL_CONFIG
