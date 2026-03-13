#include <stdbigos/sbi.h>
#include <stdbigos/types.h>

enum {
	SBI_FID_BASE_GET_SPEC_VERSION = 0,
	SBI_FID_BASE_GET_IMPL_ID = 1,
	SBI_FID_BASE_GET_IMPL_VERSION = 2,
	SBI_FID_BASE_PROBE_EXTENSION = 3,
	SBI_FID_BASE_GET_MVENDORID = 4,
	SBI_FID_BASE_GET_MARCHID = 5,
	SBI_FID_BASE_GET_MIMPID = 6,
};

enum {
	SBI_FID_TIME_SET_TIMER = 0,
};

enum {
	SBI_FID_IPI_SEND_IPI = 0,
};

enum {
	SBI_FID_RFENCE_REMOTE_FENCE_I = 0,
	SBI_FID_RFENCE_REMOTE_SFENCE_VMA = 1,
	SBI_FID_RFENCE_REMOTE_SFENCE_VMA_ASID = 2,
	SBI_FID_RFENCE_REMOTE_HFENCE_GVMA_VMID = 3,
	SBI_FID_RFENCE_REMOTE_HFENCE_GVMA = 4,
	SBI_FID_RFENCE_REMOTE_HFENCE_VVMA_ASID = 5,
	SBI_FID_RFENCE_REMOTE_HFENCE_VVMA = 6,
};

enum {
	SBI_FID_HSM_HART_START = 0,
	SBI_FID_HSM_HART_STOP = 1,
	SBI_FID_HSM_HART_GET_STATUS = 2,
	SBI_FID_HSM_HART_SUSPEND = 3,
};

enum {
	SBI_FID_SRST_SYSTEM_RESET = 0,
};

enum {
	SBI_FID_PMU_NUM_COUNTERS = 0,
	SBI_FID_PMU_COUNTER_GET_INFO = 1,
	SBI_FID_PMU_COUNTER_CONFIG_MATCHING = 2,
	SBI_FID_PMU_COUNTER_START = 3,
	SBI_FID_PMU_COUNTER_STOP = 4,
	SBI_FID_PMU_COUNTER_FW_READ = 5,
	SBI_FID_PMU_COUNTER_FW_READ_HI = 6,
	SBI_FID_PMU_SNAPSHOT_SET_SHMEM = 7,
	SBI_FID_PMU_EVENT_GET_INFO = 8,
};

enum {
	SBI_FID_DBCN_CONSOLE_WRITE = 0,
	SBI_FID_DBCN_CONSOLE_READ = 1,
	SBI_FID_DBCN_CONSOLE_WRITE_BYTE = 2,
};

enum {
	SBI_FID_SUSP_SYSTEM_SUSPEND = 0,
};

enum {
	SBI_FID_CPPC_PROBE = 0,
	SBI_FID_CPPC_READ = 1,
	SBI_FID_CPPC_READ_HI = 2,
	SBI_FID_CPPC_WRITE = 3,
};

enum {
	SBI_FID_NACL_PROBE_FEATURE = 0,
	SBI_FID_NACL_SET_SHMEM = 1,
	SBI_FID_NACL_SYNC_CSR = 2,
	SBI_FID_NACL_SYNC_HFENCE = 3,
	SBI_FID_NACL_SYNC_SRET = 4,
};

enum {
	SBI_FID_STA_SET_SHMEM = 0,
};

enum {
	SBI_FID_SSE_READ_ATTRS = 0,
	SBI_FID_SSE_WRITE_ATTRS = 1,
	SBI_FID_SSE_REGISTER = 2,
	SBI_FID_SSE_UNREGISTER = 3,
	SBI_FID_SSE_ENABLE = 4,
	SBI_FID_SSE_DISABLE = 5,
	SBI_FID_SSE_COMPLETE = 6,
	SBI_FID_SSE_INJECT = 7,
	SBI_FID_SSE_HART_UNMASK = 8,
	SBI_FID_SSE_HART_MASK = 9,
};

enum {
	SBI_FID_FWFT_SET = 0,
	SBI_FID_FWFT_GET = 1,
};

enum {
	SBI_FID_DBTR_NUM_TRIGGERS = 0,
	SBI_FID_DBTR_SET_SHMEM = 1,
	SBI_FID_DBTR_READ_TRIGGERS = 2,
	SBI_FID_DBTR_INSTALL_TRIGGERS = 3,
	SBI_FID_DBTR_UPDATE_TRIGGERS = 4,
	SBI_FID_DBTR_UNINSTALL_TRIGGERS = 5,
	SBI_FID_DBTR_ENABLE_TRIGGERS = 6,
	SBI_FID_DBTR_DISABLE_TRIGGERS = 7,
};

enum {
	SBI_FID_MPXY_GET_SHMEM_SIZE = 0,
	SBI_FID_MPXY_SET_SHMEM = 1,
	SBI_FID_MPXY_GET_CHANNEL_IDS = 2,
	SBI_FID_MPXY_READ_ATTRIBUTES = 3,
	SBI_FID_MPXY_WRITE_ATTRIBUTES = 4,
	SBI_FID_MPXY_SEND_MESSAGE_WITH_RESPONSE = 5,
	SBI_FID_MPXY_SEND_MESSAGE_WITHOUT_RESPONSE = 6,
	SBI_FID_MPXY_GET_NOTIFICATION_EVENTS = 7,
};

/*
 * Low-level SBI ecall wrapper.
 *
 * ABI:
 *  a0..a5 : args
 *  a6     : fid
 *  a7     : ext
 *
 * Returns sbiret where:
 *  - error is returned in a0
 *  - value/uvalue is returned in a1
 */
struct sbiret sbi_ecall(reg_t arg0, reg_t arg1, reg_t arg2, reg_t arg3, reg_t arg4, reg_t arg5, ireg_t fid,
                        sbi_extension_id_t ext) {
	register reg_t a0 __asm__("a0") = arg0;
	register reg_t a1 __asm__("a1") = arg1;
	register reg_t a2 __asm__("a2") = arg2;
	register reg_t a3 __asm__("a3") = arg3;
	register reg_t a4 __asm__("a4") = arg4;
	register reg_t a5 __asm__("a5") = arg5;
	register ireg_t a6 __asm__("a6") = fid;
	register ireg_t a7 __asm__("a7") = (ireg_t)ext;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7) : "memory");
	return (struct sbiret){.error = (sbi_error_t)(ireg_t)a0, .value = (ireg_t)a1};
}

struct sbiret sbi_get_spec_version(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_BASE_GET_SPEC_VERSION, SBI_EXT_BASE);
}

struct sbiret sbi_get_impl_id(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_BASE_GET_IMPL_ID, SBI_EXT_BASE);
}

struct sbiret sbi_get_impl_version(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_BASE_GET_IMPL_VERSION, SBI_EXT_BASE);
}

struct sbiret sbi_probe_extension(sbi_extension_id_t ext) {
	return sbi_ecall((reg_t)ext, 0, 0, 0, 0, 0, SBI_FID_BASE_PROBE_EXTENSION, SBI_EXT_BASE);
}

struct sbiret sbi_get_mvendorid(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_BASE_GET_MVENDORID, SBI_EXT_BASE);
}

struct sbiret sbi_get_marchid(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_BASE_GET_MARCHID, SBI_EXT_BASE);
}

struct sbiret sbi_get_mimpid(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_BASE_GET_MIMPID, SBI_EXT_BASE);
}

struct sbiret sbi_legacy_set_timer(u64 stime_value) {
#if __riscv_xlen >= 64
	return sbi_ecall((reg_t)stime_value, 0, 0, 0, 0, 0, 0, SBI_EXT_LEGACY_SET_TIMER);
#elif __riscv_xlen == 32
	return sbi_ecall((reg_t)(stime_value & 0xFFFFFFFFu), (reg_t)(stime_value >> 32), 0, 0, 0, 0, 0,
	                 SBI_EXT_LEGACY_SET_TIMER);
#else
	#error "Unsupported RISC-V XLEN"
#endif
}

struct sbiret sbi_legacy_console_putchar(reg_t ch) {
	return sbi_ecall(ch, 0, 0, 0, 0, 0, 0, SBI_EXT_LEGACY_CONSOLE_PUTCHAR);
}

struct sbiret sbi_legacy_console_getchar(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, 0, SBI_EXT_LEGACY_CONSOLE_GETCHAR);
}

struct sbiret sbi_legacy_clear_ipi(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, 0, SBI_EXT_LEGACY_CLEAR_IPI);
}

struct sbiret sbi_legacy_send_ipi(reg_t hart_mask) {
	return sbi_ecall(hart_mask, 0, 0, 0, 0, 0, 0, SBI_EXT_LEGACY_SEND_IPI);
}

struct sbiret sbi_legacy_remote_fence_i(reg_t hart_mask) {
	return sbi_ecall(hart_mask, 0, 0, 0, 0, 0, 0, SBI_EXT_LEGACY_REMOTE_FENCE_I);
}

struct sbiret sbi_legacy_remote_sfence_vma(reg_t hart_mask, reg_t start_addr, reg_t size) {
	return sbi_ecall(hart_mask, start_addr, size, 0, 0, 0, 0, SBI_EXT_LEGACY_REMOTE_SFENCE_VMA);
}

struct sbiret sbi_legacy_remote_sfence_vma_asid(reg_t hart_mask, reg_t start_addr, reg_t size, reg_t asid) {
	return sbi_ecall(hart_mask, start_addr, size, asid, 0, 0, 0, SBI_EXT_LEGACY_REMOTE_SFENCE_VMA_ASID);
}

struct sbiret sbi_legacy_shutdown(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, 0, SBI_EXT_LEGACY_SHUTDOWN);
}

struct sbiret sbi_set_timer(u64 stime_value) {
#if __riscv_xlen >= 64
	return sbi_ecall((reg_t)stime_value, 0, 0, 0, 0, 0, SBI_FID_TIME_SET_TIMER, SBI_EXT_TIME);
#elif __riscv_xlen == 32
	return sbi_ecall((reg_t)(stime_value & 0xFFFFFFFFu), (reg_t)(stime_value >> 32), 0, 0, 0, 0, SBI_FID_TIME_SET_TIMER,
	                 SBI_EXT_TIME);
#else
	#error "Unsupported RISC-V XLEN"
#endif
}

struct sbiret sbi_send_ipi(reg_t hart_mask, reg_t hart_mask_base) {
	return sbi_ecall(hart_mask, hart_mask_base, 0, 0, 0, 0, SBI_FID_IPI_SEND_IPI, SBI_EXT_IPI);
}

struct sbiret sbi_remote_fence_i(reg_t hart_mask, reg_t hart_mask_base) {
	return sbi_ecall(hart_mask, hart_mask_base, 0, 0, 0, 0, SBI_FID_RFENCE_REMOTE_FENCE_I, SBI_EXT_RFENCE);
}

struct sbiret sbi_remote_sfence_vma(reg_t hart_mask, reg_t hart_mask_base, const void* start_addr, reg_t size) {
	return sbi_ecall(hart_mask, hart_mask_base, (reg_t)start_addr, size, 0, 0, SBI_FID_RFENCE_REMOTE_SFENCE_VMA,
	                 SBI_EXT_RFENCE);
}

struct sbiret sbi_remote_sfence_vma_asid(reg_t hart_mask, reg_t hart_mask_base, const void* start_addr, reg_t size,
                                         reg_t asid) {
	return sbi_ecall(hart_mask, hart_mask_base, (reg_t)start_addr, size, asid, 0, SBI_FID_RFENCE_REMOTE_SFENCE_VMA_ASID,
	                 SBI_EXT_RFENCE);
}

struct sbiret sbi_remote_hfence_gvma_vmid(reg_t hart_mask, reg_t hart_mask_base, const void* start_addr, reg_t size,
                                          reg_t vmid) {
	return sbi_ecall(hart_mask, hart_mask_base, (reg_t)start_addr, size, vmid, 0,
	                 SBI_FID_RFENCE_REMOTE_HFENCE_GVMA_VMID, SBI_EXT_RFENCE);
}

struct sbiret sbi_remote_hfence_gvma(reg_t hart_mask, reg_t hart_mask_base, const void* start_addr, reg_t size) {
	return sbi_ecall(hart_mask, hart_mask_base, (reg_t)start_addr, size, 0, 0, SBI_FID_RFENCE_REMOTE_HFENCE_GVMA,
	                 SBI_EXT_RFENCE);
}

struct sbiret sbi_remote_hfence_vvma_asid(reg_t hart_mask, reg_t hart_mask_base, const void* start_addr, reg_t size,
                                          reg_t asid) {
	return sbi_ecall(hart_mask, hart_mask_base, (reg_t)start_addr, size, asid, 0,
	                 SBI_FID_RFENCE_REMOTE_HFENCE_VVMA_ASID, SBI_EXT_RFENCE);
}

struct sbiret sbi_remote_hfence_vvma(reg_t hart_mask, reg_t hart_mask_base, const void* start_addr, reg_t size) {
	return sbi_ecall(hart_mask, hart_mask_base, (reg_t)start_addr, size, 0, 0, SBI_FID_RFENCE_REMOTE_HFENCE_VVMA,
	                 SBI_EXT_RFENCE);
}

struct sbiret sbi_hart_start(reg_t hart_id, reg_t start_addr, reg_t opaque) {
	return sbi_ecall(hart_id, start_addr, opaque, 0, 0, 0, SBI_FID_HSM_HART_START, SBI_EXT_HSM);
}

struct sbiret sbi_hart_stop(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_HSM_HART_STOP, SBI_EXT_HSM);
}

struct sbiret sbi_hart_get_status(reg_t hart_id) {
	return sbi_ecall(hart_id, 0, 0, 0, 0, 0, SBI_FID_HSM_HART_GET_STATUS, SBI_EXT_HSM);
}

struct sbiret sbi_hart_suspend(sbi_hsm_suspend_type_t suspend_type, reg_t resume_addr, reg_t opaque) {
	return sbi_ecall((reg_t)suspend_type, resume_addr, opaque, 0, 0, 0, SBI_FID_HSM_HART_SUSPEND, SBI_EXT_HSM);
}

struct sbiret sbi_hart_suspend_nonretentive(reg_t resume_addr, reg_t opaque) {
	return sbi_hart_suspend(SBI_HSM_SUSPEND_DEFAULT_NON_RETENTIVE, resume_addr, opaque);
}

struct sbiret sbi_hart_suspend_retentive(void) {
	return sbi_hart_suspend(SBI_HSM_SUSPEND_DEFAULT_RETENTIVE, 0, 0);
}

struct sbiret sbi_system_reset(sbi_reset_type_t reset_type, sbi_reset_reason_t reset_reason) {
	return sbi_ecall((reg_t)reset_type, (reg_t)reset_reason, 0, 0, 0, 0, SBI_FID_SRST_SYSTEM_RESET,
	                 SBI_EXT_SYSTEM_RESET);
}

struct sbiret sbi_pmu_num_counters(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_PMU_NUM_COUNTERS, SBI_EXT_PMU);
}

struct sbiret sbi_pmu_counter_get_info(reg_t counter_idx) {
	return sbi_ecall(counter_idx, 0, 0, 0, 0, 0, SBI_FID_PMU_COUNTER_GET_INFO, SBI_EXT_PMU);
}

struct sbiret sbi_pmu_counter_config_matching(reg_t counter_idx_base, reg_t counter_idx_mask, reg_t config_flags,
                                              reg_t event_idx, u64 event_data) {
#if __riscv_xlen >= 64
	return sbi_ecall(counter_idx_base, counter_idx_mask, config_flags, event_idx, (reg_t)event_data, 0,
	                 SBI_FID_PMU_COUNTER_CONFIG_MATCHING, SBI_EXT_PMU);
#elif __riscv_xlen == 32
	return sbi_ecall(counter_idx_base, counter_idx_mask, config_flags, event_idx, (reg_t)(event_data & 0xFFFFFFFFu),
	                 (reg_t)(event_data >> 32), SBI_FID_PMU_COUNTER_CONFIG_MATCHING, SBI_EXT_PMU);
#else
	#error "Unsupported RISC-V XLEN"
#endif
}

struct sbiret sbi_pmu_counter_start(reg_t counter_idx_base, reg_t counter_idx_mask, reg_t start_flags,
                                    u64 initial_value) {
#if __riscv_xlen >= 64
	return sbi_ecall(counter_idx_base, counter_idx_mask, start_flags, (reg_t)initial_value, 0, 0,
	                 SBI_FID_PMU_COUNTER_START, SBI_EXT_PMU);
#elif __riscv_xlen == 32
	return sbi_ecall(counter_idx_base, counter_idx_mask, start_flags, (reg_t)(initial_value & 0xFFFFFFFFu),
	                 (reg_t)(initial_value >> 32), 0, SBI_FID_PMU_COUNTER_START, SBI_EXT_PMU);
#else
	#error "Unsupported RISC-V XLEN"
#endif
}

struct sbiret sbi_pmu_counter_stop(reg_t counter_idx_base, reg_t counter_idx_mask, reg_t stop_flags) {
	return sbi_ecall(counter_idx_base, counter_idx_mask, stop_flags, 0, 0, 0, SBI_FID_PMU_COUNTER_STOP, SBI_EXT_PMU);
}

struct sbiret sbi_pmu_counter_fw_read(reg_t counter_idx) {
	return sbi_ecall(counter_idx, 0, 0, 0, 0, 0, SBI_FID_PMU_COUNTER_FW_READ, SBI_EXT_PMU);
}

struct sbiret sbi_pmu_counter_fw_read_hi(reg_t counter_idx) {
	return sbi_ecall(counter_idx, 0, 0, 0, 0, 0, SBI_FID_PMU_COUNTER_FW_READ_HI, SBI_EXT_PMU);
}

struct sbiret sbi_pmu_snapshot_set_shmem(reg_t shmem_phys_lo, reg_t shmem_phys_hi, reg_t flags) {
	return sbi_ecall(shmem_phys_lo, shmem_phys_hi, flags, 0, 0, 0, SBI_FID_PMU_SNAPSHOT_SET_SHMEM, SBI_EXT_PMU);
}

struct sbiret sbi_pmu_event_get_info(reg_t shmem_phys_lo, reg_t shmem_phys_hi, reg_t num_entries, reg_t flags) {
	return sbi_ecall(shmem_phys_lo, shmem_phys_hi, num_entries, flags, 0, 0, SBI_FID_PMU_EVENT_GET_INFO, SBI_EXT_PMU);
}

struct sbiret sbi_debug_console_write(reg_t num_bytes, reg_t base_addr_lo, reg_t base_addr_hi) {
	return sbi_ecall(num_bytes, base_addr_lo, base_addr_hi, 0, 0, 0, SBI_FID_DBCN_CONSOLE_WRITE, SBI_EXT_DBCN);
}

struct sbiret sbi_debug_console_read(reg_t num_bytes, reg_t base_addr_lo, reg_t base_addr_hi) {
	return sbi_ecall(num_bytes, base_addr_lo, base_addr_hi, 0, 0, 0, SBI_FID_DBCN_CONSOLE_READ, SBI_EXT_DBCN);
}

struct sbiret sbi_debug_console_write_byte(u8 byte) {
	return sbi_ecall((reg_t)byte, 0, 0, 0, 0, 0, SBI_FID_DBCN_CONSOLE_WRITE_BYTE, SBI_EXT_DBCN);
}

struct sbiret sbi_system_suspend(u32 sleep_type, reg_t resume_addr, reg_t opaque) {
	return sbi_ecall((reg_t)sleep_type, resume_addr, opaque, 0, 0, 0, SBI_FID_SUSP_SYSTEM_SUSPEND, SBI_EXT_SUSP);
}

struct sbiret sbi_cppc_probe(u32 cppc_reg_id) {
	return sbi_ecall((reg_t)cppc_reg_id, 0, 0, 0, 0, 0, SBI_FID_CPPC_PROBE, SBI_EXT_CPPC);
}

struct sbiret sbi_cppc_read(u32 cppc_reg_id) {
	return sbi_ecall((reg_t)cppc_reg_id, 0, 0, 0, 0, 0, SBI_FID_CPPC_READ, SBI_EXT_CPPC);
}

struct sbiret sbi_cppc_read_hi(u32 cppc_reg_id) {
	return sbi_ecall((reg_t)cppc_reg_id, 0, 0, 0, 0, 0, SBI_FID_CPPC_READ_HI, SBI_EXT_CPPC);
}

struct sbiret sbi_cppc_write(u32 cppc_reg_id, u64 val) {
#if __riscv_xlen >= 64
	return sbi_ecall((reg_t)cppc_reg_id, (reg_t)val, 0, 0, 0, 0, SBI_FID_CPPC_WRITE, SBI_EXT_CPPC);
#elif __riscv_xlen == 32
	return sbi_ecall((reg_t)cppc_reg_id, (reg_t)(val & 0xFFFFFFFFu), (reg_t)(val >> 32), 0, 0, 0, SBI_FID_CPPC_WRITE,
	                 SBI_EXT_CPPC);
#else
	#error "Unsupported RISC-V XLEN"
#endif
}

struct sbiret sbi_nacl_probe_feature(u32 feature_id) {
	return sbi_ecall((reg_t)feature_id, 0, 0, 0, 0, 0, SBI_FID_NACL_PROBE_FEATURE, SBI_EXT_NACL);
}

struct sbiret sbi_nacl_set_shmem(reg_t shmem_phys_lo, reg_t shmem_phys_hi, reg_t flags) {
	return sbi_ecall(shmem_phys_lo, shmem_phys_hi, flags, 0, 0, 0, SBI_FID_NACL_SET_SHMEM, SBI_EXT_NACL);
}

struct sbiret sbi_nacl_sync_csr(reg_t csr_num) {
	return sbi_ecall(csr_num, 0, 0, 0, 0, 0, SBI_FID_NACL_SYNC_CSR, SBI_EXT_NACL);
}

struct sbiret sbi_nacl_sync_hfence(reg_t entry_index) {
	return sbi_ecall(entry_index, 0, 0, 0, 0, 0, SBI_FID_NACL_SYNC_HFENCE, SBI_EXT_NACL);
}

struct sbiret sbi_nacl_sync_sret(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_NACL_SYNC_SRET, SBI_EXT_NACL);
}

struct sbiret sbi_steal_time_set_shmem(reg_t shmem_phys_lo, reg_t shmem_phys_hi, reg_t flags) {
	return sbi_ecall(shmem_phys_lo, shmem_phys_hi, flags, 0, 0, 0, SBI_FID_STA_SET_SHMEM, SBI_EXT_STA);
}

struct sbiret sbi_sse_read_attrs(u32 event_id, u32 base_attr_id, u32 attr_count, reg_t output_phys_lo,
                                 reg_t output_phys_hi) {
	return sbi_ecall((reg_t)event_id, (reg_t)base_attr_id, (reg_t)attr_count, output_phys_lo, output_phys_hi, 0,
	                 SBI_FID_SSE_READ_ATTRS, SBI_EXT_SSE);
}

struct sbiret sbi_sse_write_attrs(u32 event_id, u32 base_attr_id, u32 attr_count, reg_t input_phys_lo,
                                  reg_t input_phys_hi) {
	return sbi_ecall((reg_t)event_id, (reg_t)base_attr_id, (reg_t)attr_count, input_phys_lo, input_phys_hi, 0,
	                 SBI_FID_SSE_WRITE_ATTRS, SBI_EXT_SSE);
}

struct sbiret sbi_sse_register(u32 event_id, reg_t handler_entry_pc, reg_t handler_entry_arg) {
	return sbi_ecall((reg_t)event_id, handler_entry_pc, handler_entry_arg, 0, 0, 0, SBI_FID_SSE_REGISTER, SBI_EXT_SSE);
}

struct sbiret sbi_sse_unregister(u32 event_id) {
	return sbi_ecall((reg_t)event_id, 0, 0, 0, 0, 0, SBI_FID_SSE_UNREGISTER, SBI_EXT_SSE);
}

struct sbiret sbi_sse_enable(u32 event_id) {
	return sbi_ecall((reg_t)event_id, 0, 0, 0, 0, 0, SBI_FID_SSE_ENABLE, SBI_EXT_SSE);
}

struct sbiret sbi_sse_disable(u32 event_id) {
	return sbi_ecall((reg_t)event_id, 0, 0, 0, 0, 0, SBI_FID_SSE_DISABLE, SBI_EXT_SSE);
}

struct sbiret sbi_sse_complete(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_SSE_COMPLETE, SBI_EXT_SSE);
}

struct sbiret sbi_sse_inject(u32 event_id, reg_t hart_id) {
	return sbi_ecall((reg_t)event_id, hart_id, 0, 0, 0, 0, SBI_FID_SSE_INJECT, SBI_EXT_SSE);
}

struct sbiret sbi_sse_hart_unmask(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_SSE_HART_UNMASK, SBI_EXT_SSE);
}

struct sbiret sbi_sse_hart_mask(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_SSE_HART_MASK, SBI_EXT_SSE);
}

struct sbiret sbi_fwft_set(u32 feature, reg_t value, reg_t flags) {
	return sbi_ecall((reg_t)feature, value, flags, 0, 0, 0, SBI_FID_FWFT_SET, SBI_EXT_FWFT);
}

struct sbiret sbi_fwft_get(u32 feature) {
	return sbi_ecall((reg_t)feature, 0, 0, 0, 0, 0, SBI_FID_FWFT_GET, SBI_EXT_FWFT);
}

struct sbiret sbi_debug_num_triggers(reg_t trig_tdata1) {
	return sbi_ecall(trig_tdata1, 0, 0, 0, 0, 0, SBI_FID_DBTR_NUM_TRIGGERS, SBI_EXT_DBTR);
}

struct sbiret sbi_debug_set_shmem(reg_t shmem_phys_lo, reg_t shmem_phys_hi, reg_t flags) {
	return sbi_ecall(shmem_phys_lo, shmem_phys_hi, flags, 0, 0, 0, SBI_FID_DBTR_SET_SHMEM, SBI_EXT_DBTR);
}

struct sbiret sbi_debug_read_triggers(reg_t trig_idx_base, reg_t trig_count) {
	return sbi_ecall(trig_idx_base, trig_count, 0, 0, 0, 0, SBI_FID_DBTR_READ_TRIGGERS, SBI_EXT_DBTR);
}

struct sbiret sbi_debug_install_triggers(reg_t trig_count) {
	return sbi_ecall(trig_count, 0, 0, 0, 0, 0, SBI_FID_DBTR_INSTALL_TRIGGERS, SBI_EXT_DBTR);
}

struct sbiret sbi_debug_update_triggers(reg_t trig_count) {
	return sbi_ecall(trig_count, 0, 0, 0, 0, 0, SBI_FID_DBTR_UPDATE_TRIGGERS, SBI_EXT_DBTR);
}

struct sbiret sbi_debug_uninstall_triggers(reg_t trig_idx_base, reg_t trig_idx_mask) {
	return sbi_ecall(trig_idx_base, trig_idx_mask, 0, 0, 0, 0, SBI_FID_DBTR_UNINSTALL_TRIGGERS, SBI_EXT_DBTR);
}

struct sbiret sbi_debug_enable_triggers(reg_t trig_idx_base, reg_t trig_idx_mask) {
	return sbi_ecall(trig_idx_base, trig_idx_mask, 0, 0, 0, 0, SBI_FID_DBTR_ENABLE_TRIGGERS, SBI_EXT_DBTR);
}

struct sbiret sbi_debug_disable_triggers(reg_t trig_idx_base, reg_t trig_idx_mask) {
	return sbi_ecall(trig_idx_base, trig_idx_mask, 0, 0, 0, 0, SBI_FID_DBTR_DISABLE_TRIGGERS, SBI_EXT_DBTR);
}

struct sbiret sbi_mpxy_get_shmem_size(void) {
	return sbi_ecall(0, 0, 0, 0, 0, 0, SBI_FID_MPXY_GET_SHMEM_SIZE, SBI_EXT_MPXY);
}

struct sbiret sbi_mpxy_set_shmem(reg_t shmem_phys_lo, reg_t shmem_phys_hi, reg_t flags) {
	return sbi_ecall(shmem_phys_lo, shmem_phys_hi, flags, 0, 0, 0, SBI_FID_MPXY_SET_SHMEM, SBI_EXT_MPXY);
}

struct sbiret sbi_mpxy_get_channel_ids(u32 start_index) {
	return sbi_ecall((reg_t)start_index, 0, 0, 0, 0, 0, SBI_FID_MPXY_GET_CHANNEL_IDS, SBI_EXT_MPXY);
}

struct sbiret sbi_mpxy_read_attributes(u32 channel_id, u32 base_attribute_id, u32 attribute_count) {
	return sbi_ecall((reg_t)channel_id, (reg_t)base_attribute_id, (reg_t)attribute_count, 0, 0, 0,
	                 SBI_FID_MPXY_READ_ATTRIBUTES, SBI_EXT_MPXY);
}

struct sbiret sbi_mpxy_write_attributes(u32 channel_id, u32 base_attribute_id, u32 attribute_count) {
	return sbi_ecall((reg_t)channel_id, (reg_t)base_attribute_id, (reg_t)attribute_count, 0, 0, 0,
	                 SBI_FID_MPXY_WRITE_ATTRIBUTES, SBI_EXT_MPXY);
}

struct sbiret sbi_mpxy_send_message_with_response(u32 channel_id, u32 message_id, reg_t message_data_len) {
	return sbi_ecall((reg_t)channel_id, (reg_t)message_id, message_data_len, 0, 0, 0,
	                 SBI_FID_MPXY_SEND_MESSAGE_WITH_RESPONSE, SBI_EXT_MPXY);
}

struct sbiret sbi_mpxy_send_message_without_response(u32 channel_id, u32 message_id, reg_t message_data_len) {
	return sbi_ecall((reg_t)channel_id, (reg_t)message_id, message_data_len, 0, 0, 0,
	                 SBI_FID_MPXY_SEND_MESSAGE_WITHOUT_RESPONSE, SBI_EXT_MPXY);
}

struct sbiret sbi_mpxy_get_notification_events(u32 channel_id) {
	return sbi_ecall((reg_t)channel_id, 0, 0, 0, 0, 0, SBI_FID_MPXY_GET_NOTIFICATION_EVENTS, SBI_EXT_MPXY);
}
