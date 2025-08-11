#include "kernel_config.h"

#include <debug/debug_stdio.h>
#include <drivers/dt/dt.h>

#include "ram_map.h"

static kernel_config_t s_kercfg = {0};
static bool s_is_set = false;

static u8 s_address_space_active_bits = 0;
static u8 s_pt_height = 0;

error_t kernel_config_set(kernel_config_t cfg) {
#ifdef __DEBUG__
	if (s_is_set)
		DEBUG_PRINTF("Kernel configuration has changed\n");
#endif
	s_kercfg = cfg;
	switch (s_kercfg.target_vms) {
	case KC_VMS_BARE:
		s_address_space_active_bits = 00;
		s_pt_height = 0;
		break;
	case KC_VMS_SV39:
		s_address_space_active_bits = 39;
		s_pt_height = 3;
		break;
	case KC_VMS_SV48:
		s_address_space_active_bits = 48;
		s_pt_height = 4;
		break;
	case KC_VMS_SV57:
		s_address_space_active_bits = 57;
		s_pt_height = 5;
		break;
	}
	s_is_set = true;
	return ERR_NONE;
}

buffer_t kernel_config_get(kercfg_field_t field) {
	if (!s_is_set)
		return (buffer_t){.data = nullptr, .size = 0, .error = BUFF_ERR_NOT_VALID};
	// if (s_is_set) // TODO: dt_init(physical_to_effective(kercfg.device_tree), little); ???
	switch (field) {
	case KERCFG_MODE:    return (buffer_t){.data = &s_kercfg.mode, .size = sizeof(s_kercfg.mode), BUFF_ERR_OK};
	case KERCFG_MACHINE: return (buffer_t){.data = &s_kercfg.machine, .size = sizeof(s_kercfg.machine), BUFF_ERR_OK};
	case KERCFG_ACTIVE_VMS:
		return (buffer_t){.data = &s_kercfg.active_vms, .size = sizeof(s_kercfg.active_vms), BUFF_ERR_OK};
	case KERCFG_TARGET_VMS:
		return (buffer_t){.data = &s_kercfg.target_vms, .size = sizeof(s_kercfg.target_vms), BUFF_ERR_OK};
	case KERCFG_ADDRESS_SPACE_BITS:
		return (buffer_t){
		    .data = &s_address_space_active_bits, .size = sizeof(s_address_space_active_bits), BUFF_ERR_OK};
	case KERCFG_PT_HEIGHT: return (buffer_t){.data = &s_pt_height, .size = sizeof(s_pt_height), BUFF_ERR_OK};
	case KERCFG_CPU_ENDIAN:
		return (buffer_t){.data = &s_kercfg.cpu_endian, .size = sizeof(s_kercfg.cpu_endian), BUFF_ERR_OK};
	default: return (buffer_t){.data = nullptr, .size = 0, .error = BUFF_ERR_NOT_VALID};
	}
}

buffer_t kernel_config_read_device_tree(const char* node_path, const char* arg_name) {
	return (buffer_t){.data = nullptr, .size = 0, .error = BUFF_ERR_NOT_VALID};
}
