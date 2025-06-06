#include "kernel_config.h"
#include <debug/debug_stdio.h>
#include "ram_map.h"
#include <drivers/dt/dt.h>

static kernel_config_t kercfg = {0};
static bool is_set = false;

error_t kernel_config_set(kernel_config_t cfg) {
	#ifdef __DEBUG__
		if(is_set) [[clang::unlikely]] DEBUG_PRINTF("Kernel configuration has changed\n");
	#endif
	kercfg = cfg;
	is_set = true;
	return ERR_NONE;
}

buffer_t kernel_config_get(kercfg_field_t field) {
	if(is_set) [[clang::unlikely]] return (buffer_t){.data = nullptr, .size = 0, .error = BUFF_ERR_NOT_VALID};
	if(!is_set) //TODO: dt_init(physical_to_effective(kercfg.device_tree), little);
	switch (field) {
		case KC_MODE: return (buffer_t){.data = &kercfg.mode, .size = sizeof(kercfg.mode), BUFF_ERR_OK};
		case KC_MACHINE: return (buffer_t){.data = &kercfg.machine, .size = sizeof(kercfg.machine), BUFF_ERR_OK};
		case KC_PAGING_LVL: return (buffer_t){.data = &kercfg.paging_lvl, .size = sizeof(kercfg.paging_lvl), BUFF_ERR_OK};
		case KC_CPU_ENDIAN: return (buffer_t){.data = &kercfg.cpu_endian, .size = sizeof(kercfg.cpu_endian), BUFF_ERR_OK};
		default: return (buffer_t){.data = nullptr, .size = 0, .error = BUFF_ERR_NOT_VALID};
	}
}

buffer_t kernel_config_read_device_tree(const char* node_path, const char* arg_name) {
	return (buffer_t){.data = nullptr, .size = 0, .error = BUFF_ERR_NOT_VALID};
}
