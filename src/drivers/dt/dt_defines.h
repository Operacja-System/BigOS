#ifndef DT_DEFINES_H
#define DT_DEFINES_H

// Magic number and offsets for the device tree

#define FDT_MAGIC                 0xd00dfeed
#define FDT_OFF_MAGIC             0x00
#define FDT_OFF_TOTAL_SIZE        0x04
#define FDT_OFF_OFF_DT_STRUCT     0x08
#define FDT_OFF_OFF_DT_STRINGS    0x0C
#define FDT_OFF_MEM_RSVMAP        0x10
#define FDT_OFF_VERSION           0x14
#define FDT_OFF_LAST_COMP_VERSION 0x18
#define FDT_OFF_BOOT_CPUID_PHYS   0x1C
#define FDT_OFF_SIZE_DT_STRINGS   0x20
#define FDT_OFF_SIZE_DT_STRUCT    0x24

#define FDT_COMPATIBLE_VERSION 17

// FDT token values
typedef enum : u32 {
	FDT_BEGIN_NODE = 0x1,
	FDT_END_NODE = 0x2,
	FDT_PROP = 0x3,
	FDT_NOP = 0x4,
	// Unused/Reserved 0x5 - 0x8
	FDT_END = 0x9
} fdt_token_t;

#endif
