#include "devtree_parser.h"
#include "devtree_access.h"
#include <klog.h>

void dt_debug_print(const void* dtb_addr) {
	dt_interactive_entity_t dtie = {0};
	error_t err = dtie_create(dtb_addr, &dtie);
	if (err) { KLOGLN_ERROR("dtree sie rozjebało (header)"); return; }
	err = dtie_goto_root_node(&dtie);
	if (err) { KLOGLN_ERROR("dtree sie rozjebało"); return; }
	KLOGLN_NOTE("TU sie nie rozjebało");
	while(dtie_goto_next_node(&dtie) != ERR_NOT_FOUND) {
		buffer_t buff = {0};
		err = dtie_get_node_name(&dtie, &buff);
		if (err) { KLOGLN_ERROR("dtree sie rozjebało"); return; }
		KLOGLN_NOTE("NODE: %s", (char*)buff.data);
		while(dtie_goto_next_property(&dtie) != ERR_NOT_FOUND) {
			buffer_t buff = {0};
			err = dtie_get_prop_name(&dtie, &buff);
			if (err) { KLOGLN_ERROR("dtree sie rozjebało"); return; }
			KLOGLN_NOTE("PROP: %s", (char*)buff.data);
		}
	}

}

