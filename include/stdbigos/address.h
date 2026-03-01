#ifndef _STDBIGOS_ADDRESS_H
#define _STDBIGOS_ADDRESS_H

#ifdef __clang__
	#define __noderef __attribute__((noderef))
	#define __phys    __attribute__((address_space(1), noderef))
	#define __user    __attribute__((address_space(2), noderef))
	#define __iomem   __attribute__((address_space(3), noderef))
#else
	#define __noderef
	#define __phys
	#define __user
	#define __iomem
#endif

#endif
