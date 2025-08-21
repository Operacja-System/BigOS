#include "interactive_entity_internal.h"

#include <stdbigos/buffer.h>
#include <stdbigos/math.h>
#include <stdbigos/string.h>
#include <klog.h>

#include "stdbigos/bitutils.h"

[[nodiscard]] static dtb_prop_header_t read_pheader(const void* addr) {
	buffer_t buff = make_buffer(addr, sizeof(dtb_prop_header_t));
	dtb_prop_header_t pheader = {0};
	(void)buffer_read_u32_be(buff, 0, &pheader.len);
	(void)buffer_read_u32_be(buff, sizeof(pheader.len), &pheader.nameof);
	return pheader;
}

[[nodiscard]] static inline dtb_token_t read_token(const void* dtb_addr) {
	dtb_token_t tok = read_be32(dtb_addr);
	return tok;
}

static inline void advance_cursor_at_token(dtb_addr_t* cursorOUT) {
	*cursorOUT += sizeof(dtb_token_t);
}

static void advance_cursor_at_node(dtb_interactive_entity_t* dtbie, dtb_addr_t* cursorOUT) {
	advance_cursor_at_token(cursorOUT);
	while (*((u8*)dtbie->dtb_addr + ++(*cursorOUT)));
	++(*cursorOUT);
	*cursorOUT = align_up_pow2(*cursorOUT, 2);
}

static void advance_cursor_at_prop(dtb_interactive_entity_t* dtbie, dtb_addr_t* cursorOUT) {
	advance_cursor_at_token(cursorOUT);
	dtb_prop_header_t pheader = read_pheader(dtbie->dtb_addr + *cursorOUT);
	*cursorOUT += sizeof(dtb_prop_header_t);
	*cursorOUT += pheader.len;
	*cursorOUT = align_up_pow2(*cursorOUT, 2);
}

[[nodiscard]] static error_t advance_cursor_until_token_int(dtb_interactive_entity_t* dtbie, dtb_addr_t* cursorOUT, dtb_token_t tok,
                                               dtb_token_t inttok) {
	bool advanced = false;
	for (u64 limit = 0; limit < DTBIE_LOOP_LIMIT; ++limit) {
		dtb_token_t token = read_token(dtbie->dtb_addr + *cursorOUT);
		if (token == inttok && advanced)
			return ERR_INTERRUPTED;
		if (token == tok && advanced)
			return ERR_NONE;
		switch (token) {
		case DTT_BEGIN_NODE: advance_cursor_at_node(dtbie, cursorOUT); break;
		case DTT_END_NODE:   advance_cursor_at_token(cursorOUT); break;
		case DTT_PROP:       advance_cursor_at_prop(dtbie, cursorOUT); break;
		case DTT_NOP:        advance_cursor_at_token(cursorOUT); break;
		case DTT_END:        return ERR_NOT_FOUND;
		}
		advanced = true;
	}
	return ERR_NOT_FOUND;
}

[[nodiscard]] static error_t push_node(dtb_interactive_entity_t* dtbie) {
	if(dtbie->cursor_stack_pointer == DTBIE_STACK_SIZE) KLOG_RETURN_ERR_TRACE(ERR_OUT_OF_BOUNDS);
	dtbie->cursor_stack[dtbie->cursor_stack_pointer++] = dtbie->node_cursor;
	return ERR_NONE;
}

[[nodiscard]] static error_t pop_node(dtb_interactive_entity_t* dtbie) {
	if(dtbie->cursor_stack_pointer == 0) KLOG_RETURN_ERR_TRACE(ERR_OUT_OF_BOUNDS);
	dtbie->node_cursor = dtbie->cursor_stack[--dtbie->cursor_stack_pointer];
	return ERR_NONE;
}

/// Public:

error_t dtbie_goto_root_node(dtb_interactive_entity_t* dtbie) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	dtbie->node_cursor = dtbie->header.off_dt_struct;
	const void* addr = dtbie->dtb_addr + dtbie->node_cursor;
	dtb_token_t token = read_token(addr);
	if (token != DTT_BEGIN_NODE) {
		dtbie->valid = false;
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	}
	dtbie->prop_cursor = dtbie->node_cursor;
	return ERR_NONE; }

error_t dtbie_goto_next_node(dtb_interactive_entity_t* dtbie) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	u64 depth = 1;
	error_t err = 0;
	do {
		err = advance_cursor_until_token_int(dtbie, &dtbie->node_cursor, DTT_END_NODE, DTT_BEGIN_NODE);
		if(err == ERR_NOT_FOUND) return err;
		if(err == ERR_INTERRUPTED) ++depth;
		else if(err == ERR_NONE) --depth;
	} while(err != ERR_NONE || depth != 0);
	err = advance_cursor_until_token_int(dtbie, &dtbie->node_cursor, DTT_BEGIN_NODE, DTT_END_NODE);
	if(err) return ERR_NOT_FOUND;
	dtbie->prop_cursor = dtbie->node_cursor;
	return ERR_NONE;
}

error_t dtbie_goto_child_node(dtb_interactive_entity_t* dtbie) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	error_t err = push_node(dtbie);
	if(err) KLOG_RETURN_ERR_TRACE(ERR_OUT_OF_BOUNDS);
	err = advance_cursor_until_token_int(dtbie, &dtbie->node_cursor, DTT_BEGIN_NODE, DTT_END_NODE);
	dtbie->prop_cursor = dtbie->node_cursor;
	if(err == ERR_NONE) return err;
	return ERR_NOT_FOUND;
}

error_t dtbie_goto_parent_node(dtb_interactive_entity_t* dtbie) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	error_t err = pop_node(dtbie);
	if (err) KLOG_RETURN_ERR_TRACE(ERR_OUT_OF_BOUNDS);
	dtbie->prop_cursor = dtbie->node_cursor;
	return ERR_NONE;
}

error_t dtbie_goto_next_prop(dtb_interactive_entity_t* dtbie) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	error_t err = advance_cursor_until_token_int(dtbie, &dtbie->prop_cursor, DTT_PROP, DTT_END_NODE);
	if(err) return ERR_NOT_FOUND;
	return ERR_NONE;
}

error_t dtbie_get_node_name(dtb_interactive_entity_t* dtbie, buffer_t* buffOUT) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	const void* addr = dtbie->dtb_addr + dtbie->node_cursor;
	dtb_token_t token = read_token(addr);
	if(token != DTT_BEGIN_NODE) KLOG_RETURN_ERR_TRACE(ERR_BAD_ARG);
	addr += sizeof(dtb_token_t);
	u64 strsize = strlen(addr) + 1;
	*buffOUT = make_buffer(addr, strsize);
	return ERR_NONE;
}

error_t dtbie_get_prop_name(dtb_interactive_entity_t* dtbie, buffer_t* buffOUT) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	const void* addr = dtbie->dtb_addr + dtbie->prop_cursor;
	dtb_token_t token = read_token(addr);
	if(token != DTT_PROP) KLOG_RETURN_ERR_TRACE(ERR_BAD_ARG);
	addr += sizeof(dtb_token_t);
	dtb_prop_header_t pheader = read_pheader(addr);
	addr = dtbie->dtb_addr + dtbie->header.off_dt_strings + pheader.nameof;
	u64 strsize = strlen(addr) + 1;
	*buffOUT = make_buffer(addr, strsize);
	return ERR_NONE;
}

error_t dtbie_get_prop_data(dtb_interactive_entity_t* dtbie, buffer_t* buffOUT) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	const void* addr = dtbie->dtb_addr + dtbie->prop_cursor;
	dtb_token_t token = read_token(addr);
	if(token != DTT_PROP) KLOG_RETURN_ERR_TRACE(ERR_BAD_ARG);
	addr += sizeof(dtb_token_t);
	dtb_prop_header_t pheader = read_pheader(addr);
	addr += sizeof(dtb_prop_header_t);
	*buffOUT = make_buffer(addr, pheader.len);
	return ERR_NONE;
}

