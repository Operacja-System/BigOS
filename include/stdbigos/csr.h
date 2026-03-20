#ifndef STDBIGOS_CSR
#define STDBIGOS_CSR

#include "csr_vals.h"
#include "types.h"

/**
 * @addtogroup stdbigos
 * @addtogroup csr
 * @{
 */

#define CSR_SWAP(csr, val)                                                               \
	({                                                                                   \
		reg_t _val = (reg_t)(val);                                                       \
		__asm__ volatile("csrrw %0, " #csr ", %1" : "=r"(_val) : "rK"(_val) : "memory"); \
		_val;                                                                            \
	})

#define CSR_READ(csr)                                                 \
	({                                                                \
		reg_t _val;                                                   \
		__asm__ volatile("csrr %0, " #csr : "=r"(_val) : : "memory"); \
		_val;                                                         \
	})

// read csr while allowing caching
#define CSR_READ_RELAXED(csr)                   \
	({                                          \
		reg_t _val;                             \
		__asm__("csrr %0, " #csr : "=r"(_val)); \
		_val;                                   \
	})

#define CSR_WRITE(csr, val)                                              \
	({                                                                   \
		reg_t _val = (reg_t)(val);                                       \
		__asm__ volatile("csrw " #csr ", %0" : : "rK"(_val) : "memory"); \
	})

#define CSR_READ_SET(csr, val)                                                           \
	({                                                                                   \
		reg_t _val = (reg_t)(val);                                                       \
		__asm__ volatile("csrrs %0, " #csr ", %1" : "=r"(_val) : "rK"(_val) : "memory"); \
		_val;                                                                            \
	})

#define CSR_READ_CLEAR(csr, val)                                                         \
	({                                                                                   \
		reg_t _val = (reg_t)(val);                                                       \
		__asm__ volatile("csrrc %0, " #csr ", %1" : "=r"(_val) : "rK"(_val) : "memory"); \
		_val;                                                                            \
	})

#define CSR_SET(csr, val)                                                \
	({                                                                   \
		reg_t _val = (reg_t)(val);                                       \
		__asm__ volatile("csrs " #csr ", %0" : : "rK"(_val) : "memory"); \
	})

#define CSR_CLEAR(csr, val)                                              \
	({                                                                   \
		reg_t _val = (reg_t)(val);                                       \
		__asm__ volatile("csrc " #csr ", %0" : : "rK"(_val) : "memory"); \
	})

static inline void wfi() {
	__asm__ volatile("wfi" ::: "memory");
}

static inline void ebreak() {
	__asm__ volatile("ebreak" ::: "memory");
}
static inline void intr_enable() {
	CSR_SET(sstatus, CSR_SSTATUS_SIE);
}
static inline void intr_disable() {
	CSR_CLEAR(sstatus, CSR_SSTATUS_SIE);
}

/// @}

#endif // !STDBIGOS_CSR
