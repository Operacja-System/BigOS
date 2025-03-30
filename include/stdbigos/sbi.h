#ifndef _STDBIGOS_SBI_H_
#define _STDBIGOS_SBI_H_

#include "types.h"

#define SBI_SPEC_VERSION_DEFAULT	 0x1
#define SBI_SPEC_VERSION_MAJOR_SHIFT 24
#define SBI_SPEC_VERSION_MAJOR_MASK	 0x7f
#define SBI_SPEC_VERSION_MINOR_MASK	 0xffffff

typedef enum : ireg_t {
	SbiSuccess = 0,
	SbiErrFailure = -1,
	SbiErrNotSupported = -2,
	SbiErrInvalidParam = -3,
	SbiErrDenied = -4,
	SbiErrInvalidAddress = -5,
	SbiErrAlreadyAvailable = -6,
	SbiErrAlreadyStarted = -7,
	SbiErrAlreadyStopped = -8,
} SbiError;

typedef enum : ireg_t {
	SbiExtBase = 0x10,
	SbiExtDebugConsole = 0x4442434E, // 'DBCN'
} SbiExtensionId;

typedef struct {
	SbiError error;
	union {
		ireg_t value;
		reg_t uvalue;
	};
} sbiret;

sbiret sbi_ecall(SbiExtensionId ext, reg_t fid, reg_t arg0, reg_t arg1, reg_t arg2, reg_t arg3, reg_t arg4, reg_t arg5);

// Base Extension

sbiret sbi_get_spec_version();
sbiret sbi_get_impl_id();
sbiret sbi_get_impl_version();
sbiret sbi_probe_extension(SbiExtensionId);
sbiret sbi_get_mvendorid();
sbiret sbi_get_marchid();
sbiret sbi_get_mimpid();

// ...

sbiret sbi_debug_console_write(reg_t num_bytes, reg_t base_addr_lo, reg_t base_addr_hi);
sbiret sbi_debug_console_read(reg_t num_bytes, reg_t base_addr_lo, reg_t base_addr_hi);
sbiret sbi_debug_console_write_byte(u8 byte);

#endif
