/**
 * @file csr.h
 * @brief Internal RISC-V CSR (Control and Status Register) access macros.
 *
 * This header is internal to HAL and should not be used by code outside the HAL.
 * CSR access is architecture-specific and not part of the platform-generic HAL interface.
 */

#ifndef HAL_ARCH_RISCV_CSR_INTERNAL_H
#define HAL_ARCH_RISCV_CSR_INTERNAL_H

#include <stdbigos/types.h>

#include "csr_vals.h"

/**
 * Read and write a CSR, swapping the old value for a new one.
 * @param csr CSR name (unquoted)
 * @param val New value
 * @return Old CSR value
 */
#define CSR_SWAP(csr, val)                                                               \
	({                                                                                   \
		reg_t _val = (reg_t)(val);                                                       \
		__asm__ volatile("csrrw %0, " #csr ", %1" : "=r"(_val) : "rK"(_val) : "memory"); \
		_val;                                                                            \
	})

/**
 * Read a CSR with full memory ordering.
 * @param csr CSR name (unquoted)
 * @return CSR value
 */
#define CSR_READ(csr)                                                 \
	({                                                                \
		reg_t _val;                                                   \
		__asm__ volatile("csrr %0, " #csr : "=r"(_val) : : "memory"); \
		_val;                                                         \
	})

/**
 * Read a CSR while allowing caching (relaxed: no memory barriers).
 * @param csr CSR name (unquoted)
 * @return CSR value
 */
#define CSR_READ_RELAXED(csr)                   \
	({                                          \
		reg_t _val;                             \
		__asm__("csrr %0, " #csr : "=r"(_val)); \
		_val;                                   \
	})

/**
 * Write a CSR with full memory ordering.
 * @param csr CSR name (unquoted)
 * @param val New value
 */
#define CSR_WRITE(csr, val)                                              \
	({                                                                   \
		reg_t _val = (reg_t)(val);                                       \
		__asm__ volatile("csrw " #csr ", %0" : : "rK"(_val) : "memory"); \
	})

/**
 * Read CSR and set bits, returning the old value.
 * @param csr CSR name (unquoted)
 * @param val Bits to set
 * @return Old CSR value
 */
#define CSR_READ_SET(csr, val)                                                           \
	({                                                                                   \
		reg_t _val = (reg_t)(val);                                                       \
		__asm__ volatile("csrrs %0, " #csr ", %1" : "=r"(_val) : "rK"(_val) : "memory"); \
		_val;                                                                            \
	})

/**
 * Read CSR and clear bits, returning the old value.
 * @param csr CSR name (unquoted)
 * @param val Bits to clear
 * @return Old CSR value
 */
#define CSR_READ_CLEAR(csr, val)                                                         \
	({                                                                                   \
		reg_t _val = (reg_t)(val);                                                       \
		__asm__ volatile("csrrc %0, " #csr ", %1" : "=r"(_val) : "rK"(_val) : "memory"); \
		_val;                                                                            \
	})

/**
 * Set bits in a CSR.
 * @param csr CSR name (unquoted)
 * @param val Bits to set
 */
#define CSR_SET(csr, val)                                                \
	({                                                                   \
		reg_t _val = (reg_t)(val);                                       \
		__asm__ volatile("csrs " #csr ", %0" : : "rK"(_val) : "memory"); \
	})

/**
 * Clear bits in a CSR.
 * @param csr CSR name (unquoted)
 * @param val Bits to clear
 */
#define CSR_CLEAR(csr, val)                                              \
	({                                                                   \
		reg_t _val = (reg_t)(val);                                       \
		__asm__ volatile("csrc " #csr ", %0" : : "rK"(_val) : "memory"); \
	})

#endif // !HAL_ARCH_RISCV_CSR_INTERNAL_H
