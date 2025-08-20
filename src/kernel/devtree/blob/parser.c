#include "parser.h"
#include "interactive_entity.h"
#include <klog.h>

void dt_debug_print(const void* dtb_addr) {
	dt_interactive_entity_t dtie = {0};
	error_t err = dtie_create(dtb_addr, &dtie);
	if (err) { KLOGLN_ERROR("dtree sie rozjebało (header) %u", err); return; }
	err = dtie_goto_root_node(&dtie);
	if (err) { KLOGLN_ERROR("dtree sie rozjebało (goto_root) %u", err); return; }
	err = dtie_goto_child_node(&dtie);
	if (err) { KLOGLN_ERROR("dtree sie rozjebało (goto next node) %u", err); return; }

	while(dtie_goto_next_node(&dtie) == ERR_NONE) {
		buffer_t buff = {0};
		err = dtie_get_node_name(&dtie, &buff);
		if (err) { KLOGLN_ERROR("dtree sie rozjebało (nodename) %u", err); return; }
		KLOGLN_NOTE("NODE: %s", (char*)buff.data);
		while(dtie_goto_next_prop(&dtie) == ERR_NONE) {
			KLOG_INDENT_BLOCK_START;
			buffer_t buff = {0};
			err = dtie_get_prop_name(&dtie, &buff);
			if (err) { KLOGLN_ERROR("dtree sie rozjebało (propname)"); return; }
			KLOGLN_NOTE("PROP: %s", (char*)buff.data);
			KLOG_INDENT_BLOCK_END;
		}
	}
}

