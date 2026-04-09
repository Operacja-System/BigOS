#ifndef HAL_RISCV_CSR_VALS
#define HAL_RISCV_CSR_VALS

#define CSR_SSTATUS_SIE        (1ul << 1)
#define CSR_SSTATUS_SPIE       (1ul << 5)
#define CSR_SSTATUS_UBE        (1ul << 6)
#define CSR_SSTATUS_SPP        (1ul << 8)
#define CSR_SSTATUS_VS_OFFSET  9
#define CSR_SSTATUS_VS_MASK    0b11
#define CSR_SSTATUS_FS_OFFSET  13
#define CSR_SSTATUS_FS_MASK    0b11
#define CSR_SSTATUS_XS_OFFSET  15
#define CSR_SSTATUS_XS_MASK    0b11
#define CSR_SSTATUS_SUM        (1ul << 18)
#define CSR_SSTATUS_MXR        (1ul << 19)
#define CSR_SSTATUS_SPELP      (1ul << 23)
#define CSR_SSTATUS_SDT        (1ul << 24)
#define CSR_SSTATUS_UXL_OFFSET 32
#define CSR_SSTATUS_UXL_MASK   0b11
#define CSR_SSTATUS_SD         (1ul << 63)

#endif // !HAL_RISCV_CSR_VALS
