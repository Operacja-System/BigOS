#ifndef _STDBIGOS_SBI_H_
#define _STDBIGOS_SBI_H_

#include "types.h"

#define SBI_SPEC_VERSION_DEFAULT     0x1
#define SBI_SPEC_VERSION_MAJOR_SHIFT 24
#define SBI_SPEC_VERSION_MAJOR_MASK  0x7f
#define SBI_SPEC_VERSION_MINOR_MASK  0xffffff

typedef enum : ireg_t {
	SBI_SUCCESS = 0,
	SBI_ERR_FAILED = -1,
	SBI_ERR_NOT_SUPPORTED = -2,
	SBI_ERR_INVALID_PARAM = -3,
	SBI_ERR_DENIED = -4,
	SBI_ERR_INVALID_ADDRESS = -5,
	SBI_ERR_ALREADY_AVAILABLE = -6,
	SBI_ERR_ALREADY_STARTED = -7,
	SBI_ERR_ALREADY_STOPPED = -8,
	SBI_ERR_NO_SHMEM = -9,
	SBI_ERR_INVALID_STATE = -10,
	SBI_ERR_BAD_RANGE = -11,
	SBI_ERR_TIMEOUT = -12,
	SBI_ERR_IO = -13,
	SBI_ERR_DENIED_LOCKED = -14,
} sbi_error_t;

typedef enum : ireg_t {
	SBI_EXT_LEGACY_SET_TIMER = 0x00,
	SBI_EXT_LEGACY_CONSOLE_PUTCHAR = 0x01,
	SBI_EXT_LEGACY_CONSOLE_GETCHAR = 0x02,
	SBI_EXT_LEGACY_CLEAR_IPI = 0x03,
	SBI_EXT_LEGACY_SEND_IPI = 0x04,
	SBI_EXT_LEGACY_REMOTE_FENCE_I = 0x05,
	SBI_EXT_LEGACY_REMOTE_SFENCE_VMA = 0x06,
	SBI_EXT_LEGACY_REMOTE_SFENCE_VMA_ASID = 0x07,
	SBI_EXT_LEGACY_SHUTDOWN = 0x08,
	SBI_EXT_BASE = 0x10,
	SBI_EXT_TIME = 0x54494D45,         // 'TIME'
	SBI_EXT_IPI = 0x735049,            // 'sPI: s-mode IPI'
	SBI_EXT_RFENCE = 0x52464E43,       // 'RFNC'
	SBI_EXT_HSM = 0x48534D,            // 'HSM'
	SBI_EXT_SYSTEM_RESET = 0x53525354, // 'SRST'
	SBI_EXT_PMU = 0x504D55,            // 'PMU'
	SBI_EXT_DBCN = 0x4442434E,         // 'DBCN'
	SBI_EXT_SUSP = 0x53555350,         // 'SUSP'
	SBI_EXT_CPPC = 0x43505043,         // 'CPPC'
	SBI_EXT_NACL = 0x4E41434C,         // 'NACL'
	SBI_EXT_STA = 0x535441,            // 'STA'
	SBI_EXT_SSE = 0x535345,            // 'SSE'
	SBI_EXT_FWFT = 0x46574654,         // 'FWFT'
	SBI_EXT_DBTR = 0x44425452,         // 'DBTR'
	SBI_EXT_MPXY = 0x4D505859,         // 'MPXY'
} sbi_extension_id_t;

typedef struct sbiret {
	sbi_error_t error;
	union {
		ireg_t value;
		reg_t uvalue;
	};
} sbiret_t;

typedef enum : reg_t {
	SBI_HSM_STARTED = 0,
	SBI_HSM_STOPPED = 1,
	SBI_HSM_START_PENDING = 2,
	SBI_HSM_STOP_PENDING = 3,
	SBI_HSM_SUSPENDED = 4,
	SBI_HSM_SUSPEND_PENDING = 5,
	SBI_HSM_RESUME_PENDING = 6,
} sbi_hsm_state_t;

typedef enum : u32 {
	SBI_HSM_SUSPEND_DEFAULT_RETENTIVE = 0,
	SBI_HSM_SUSPEND_DEFAULT_NON_RETENTIVE = 0x80000000,
} sbi_hsm_suspend_type_t;

typedef enum : u32 {
	SBI_RESET_TYPE_SHUTDOWN = 0,
	SBI_RESET_TYPE_COLD_REBOOT = 1,
	SBI_RESET_TYPE_WARM_REBOOT = 2,
} sbi_reset_type_t;

typedef enum : u32 {
	SBI_RESET_REASON_NO_REASON = 0,
	SBI_RESET_REASON_SYSTEM_FAILURE = 1,
} sbi_reset_reason_t;

struct sbiret sbi_ecall(reg_t arg0, reg_t arg1, reg_t arg2, reg_t arg3, reg_t arg4, reg_t arg5, ireg_t fid,
                        sbi_extension_id_t ext);

struct sbiret sbi_get_spec_version(void);
struct sbiret sbi_get_impl_id(void);
struct sbiret sbi_get_impl_version(void);
struct sbiret sbi_probe_extension(sbi_extension_id_t);
struct sbiret sbi_get_mvendorid(void);
struct sbiret sbi_get_marchid(void);
struct sbiret sbi_get_mimpid(void);

struct sbiret sbi_legacy_set_timer(u64 stime_value);
struct sbiret sbi_legacy_console_putchar(reg_t ch);
struct sbiret sbi_legacy_console_getchar(void);
struct sbiret sbi_legacy_clear_ipi(void);
struct sbiret sbi_legacy_send_ipi(reg_t hart_mask);
struct sbiret sbi_legacy_remote_fence_i(reg_t hart_mask);
struct sbiret sbi_legacy_remote_sfence_vma(reg_t hart_mask, reg_t start_addr, reg_t size);
struct sbiret sbi_legacy_remote_sfence_vma_asid(reg_t hart_mask, reg_t start_addr, reg_t size, reg_t asid);
struct sbiret sbi_legacy_shutdown(void);

struct sbiret sbi_set_timer(u64 stime_value);

struct sbiret sbi_send_ipi(reg_t hart_mask, reg_t hart_mask_base);

struct sbiret sbi_remote_fence_i(reg_t hart_mask, reg_t hart_mask_base);
struct sbiret sbi_remote_sfence_vma(reg_t hart_mask, reg_t hart_mask_base, const void* start_addr, reg_t size);
struct sbiret sbi_remote_sfence_vma_asid(reg_t hart_mask, reg_t hart_mask_base, const void* start_addr, reg_t size,
                                         reg_t asid);
struct sbiret sbi_remote_hfence_gvma_vmid(reg_t hart_mask, reg_t hart_mask_base, const void* start_addr, reg_t size,
                                          reg_t vmid);
struct sbiret sbi_remote_hfence_gvma(reg_t hart_mask, reg_t hart_mask_base, const void* start_addr, reg_t size);
struct sbiret sbi_remote_hfence_vvma_asid(reg_t hart_mask, reg_t hart_mask_base, const void* start_addr, reg_t size,
                                          reg_t asid);
struct sbiret sbi_remote_hfence_vvma(reg_t hart_mask, reg_t hart_mask_base, const void* start_addr, reg_t size);

struct sbiret sbi_hart_start(reg_t hart_id, reg_t start_addr, reg_t opaque);
struct sbiret sbi_hart_suspend(sbi_hsm_suspend_type_t suspend_type, reg_t resume_addr, reg_t opaque);
struct sbiret sbi_hart_get_status(reg_t hart_id);
// SAFETY: must be called with supervisor-mode interrupts disabled
struct sbiret sbi_hart_stop();

struct sbiret sbi_hart_suspend_nonretentive(reg_t resume_addr, reg_t opaque);
struct sbiret sbi_hart_suspend_retentive();

struct sbiret sbi_system_reset(sbi_reset_type_t reset_type, sbi_reset_reason_t reset_reason);

struct sbiret sbi_pmu_num_counters(void);
struct sbiret sbi_pmu_counter_get_info(reg_t counter_idx);
struct sbiret sbi_pmu_counter_config_matching(reg_t counter_idx_base, reg_t counter_idx_mask, reg_t config_flags,
                                              reg_t event_idx, u64 event_data);
struct sbiret sbi_pmu_counter_start(reg_t counter_idx_base, reg_t counter_idx_mask, reg_t start_flags,
                                    u64 initial_value);
struct sbiret sbi_pmu_counter_stop(reg_t counter_idx_base, reg_t counter_idx_mask, reg_t stop_flags);
struct sbiret sbi_pmu_counter_fw_read(reg_t counter_idx);
struct sbiret sbi_pmu_counter_fw_read_hi(reg_t counter_idx);
struct sbiret sbi_pmu_snapshot_set_shmem(reg_t shmem_phys_lo, reg_t shmem_phys_hi, reg_t flags);
struct sbiret sbi_pmu_event_get_info(reg_t shmem_phys_lo, reg_t shmem_phys_hi, reg_t num_entries, reg_t flags);

struct sbiret sbi_debug_console_write(reg_t num_bytes, reg_t base_addr_lo, reg_t base_addr_hi);
struct sbiret sbi_debug_console_read(reg_t num_bytes, reg_t base_addr_lo, reg_t base_addr_hi);
struct sbiret sbi_debug_console_write_byte(u8 byte);

struct sbiret sbi_system_suspend(u32 sleep_type, reg_t resume_addr, reg_t opaque);

struct sbiret sbi_cppc_probe(u32 cppc_reg_id);
struct sbiret sbi_cppc_read(u32 cppc_reg_id);
struct sbiret sbi_cppc_read_hi(u32 cppc_reg_id);
struct sbiret sbi_cppc_write(u32 cppc_reg_id, u64 val);

struct sbiret sbi_nacl_probe_feature(u32 feature_id);
struct sbiret sbi_nacl_set_shmem(reg_t shmem_phys_lo, reg_t shmem_phys_hi, reg_t flags);
struct sbiret sbi_nacl_sync_csr(reg_t csr_num);
struct sbiret sbi_nacl_sync_hfence(reg_t entry_index);
struct sbiret sbi_nacl_sync_sret(void);

struct sbiret sbi_steal_time_set_shmem(reg_t shmem_phys_lo, reg_t shmem_phys_hi, reg_t flags);

struct sbiret sbi_sse_read_attrs(u32 event_id, u32 base_attr_id, u32 attr_count, reg_t output_phys_lo,
                                 reg_t output_phys_hi);
struct sbiret sbi_sse_write_attrs(u32 event_id, u32 base_attr_id, u32 attr_count, reg_t input_phys_lo,
                                  reg_t input_phys_hi);
struct sbiret sbi_sse_register(u32 event_id, reg_t handler_entry_pc, reg_t handler_entry_arg);
struct sbiret sbi_sse_unregister(u32 event_id);
struct sbiret sbi_sse_enable(u32 event_id);
struct sbiret sbi_sse_disable(u32 event_id);
struct sbiret sbi_sse_complete(void);
struct sbiret sbi_sse_inject(u32 event_id, reg_t hart_id);
struct sbiret sbi_sse_hart_unmask(void);
struct sbiret sbi_sse_hart_mask(void);

struct sbiret sbi_fwft_set(u32 feature, reg_t value, reg_t flags);
struct sbiret sbi_fwft_get(u32 feature);

struct sbiret sbi_debug_num_triggers(reg_t trig_tdata1);
struct sbiret sbi_debug_set_shmem(reg_t shmem_phys_lo, reg_t shmem_phys_hi, reg_t flags);
struct sbiret sbi_debug_read_triggers(reg_t trig_idx_base, reg_t trig_count);
struct sbiret sbi_debug_install_triggers(reg_t trig_count);
struct sbiret sbi_debug_update_triggers(reg_t trig_count);
struct sbiret sbi_debug_uninstall_triggers(reg_t trig_idx_base, reg_t trig_idx_mask);
struct sbiret sbi_debug_enable_triggers(reg_t trig_idx_base, reg_t trig_idx_mask);
struct sbiret sbi_debug_disable_triggers(reg_t trig_idx_base, reg_t trig_idx_mask);

struct sbiret sbi_mpxy_get_shmem_size(void);
struct sbiret sbi_mpxy_set_shmem(reg_t shmem_phys_lo, reg_t shmem_phys_hi, reg_t flags);
struct sbiret sbi_mpxy_get_channel_ids(u32 start_index);
struct sbiret sbi_mpxy_read_attributes(u32 channel_id, u32 base_attribute_id, u32 attribute_count);
struct sbiret sbi_mpxy_write_attributes(u32 channel_id, u32 base_attribute_id, u32 attribute_count);
struct sbiret sbi_mpxy_send_message_with_response(u32 channel_id, u32 message_id, reg_t message_data_len);
struct sbiret sbi_mpxy_send_message_without_response(u32 channel_id, u32 message_id, reg_t message_data_len);
struct sbiret sbi_mpxy_get_notification_events(u32 channel_id);

#endif
