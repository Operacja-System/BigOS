#include <hal/hal.h>

#include "dt/dt.h"

//===========
// Internals
//===========

static bool g_is_init = false;
static __phys void* g_dtb = nullptr;
static void* (*g_physical_to_effective)(__phys void*) = nullptr;

bool ihal_is_init() {
	return g_is_init;
}

error_t ihal_get_dtb(void** dtbOUT) {
	if (!g_is_init)
		return ERR_NOT_INITIALIZED;
	*dtbOUT = g_physical_to_effective(g_dtb);
	return ERR_NONE;
}

//========
// Public
//========

error_t hal_init(__phys void* dtb, void* (*physical_to_effective)(__phys void*)) {
	if (dtb == nullptr)
		return ERR_BAD_ARG;
	g_dtb = dtb;
	fdt_t fdt = {0};
	// Check if dtb is valid device tree
	error_t err = dt_init(physical_to_effective(dtb), &fdt);
	if (err)
		return err;
	g_physical_to_effective = physical_to_effective;
	g_is_init = true;
	return ERR_NONE;
}
