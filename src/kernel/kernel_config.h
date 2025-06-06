#ifndef BIGOS_KERNEL_KERNEL_CONFIG
#define BIGOS_KERNEL_KERNEL_CONFIG

#include <stdbigos/types.h>
#include <stdbigos/buffer.h>
#include "memory_managment/physical_memory_manager.h"

typedef enum : u8 {
	KC_MODE_32 = 32,
	KC_MODE_64 = 64,
} kerconf_mode_t;

typedef enum : u8 {
	KC_MACHINE_RISCV = 1,
	KC_MACHINE_X86 = 2,
	KC_MACHINE_ARM = 3,
} kerconf_machine_t;

typedef enum : u8 {
	KC_PAGING_LVL_3 = 3,
	KC_PAGING_LVL_4 = 4,
	KC_PAGING_LVL_5 = 5,
} kerconf_paging_level_t;

typedef enum : u8 {
	KC_CPU_ENDIAN_LITTLE_ENDIAN = 1,
	KC_CPU_ENDIAN_BIG_ENDIAN = 2,
} kerconf_cpu_endian_t;

typedef struct {
	kerconf_mode_t mode;
	kerconf_machine_t machine;
	kerconf_paging_level_t paging_lvl;
	kerconf_cpu_endian_t cpu_endian;
	phys_addr_t device_tree_phys_addr;
} kernel_config_t;

error_t kernel_config_set(kernel_config_t cfg);

typedef enum {
	KC_MODE,
	KC_MACHINE,
	KC_PAGING_LVL,
	KC_CPU_ENDIAN,
} kercfg_field_t;

buffer_t kernel_config_get(kercfg_field_t field);
buffer_t kernel_config_read_device_tree(const char* node_path, const char* arg_name);

#endif // !BIGOS_KERNEL_KERNEL_CONFIG
