#include "interactive_entity.h"

#include <stdbigos/buffer.h>
#include <stdbigos/math.h>
#include <stdbigos/string.h>
#include <klog.h>

#include "stdbigos/bitutils.h"

typedef enum : u32 {
	DTT_BEGIN_NODE = 0x1,
	DTT_END_NODE = 0x2,
	DTT_PROP = 0x3,
	DTT_NOP = 0x4,
	// yup, there is a gap here
	DTT_END = 0x9,
} dt_token_t;

typedef struct {
	u32 len;
	u32 nameof;
} dt_prop_header_t;

[[nodiscard]] static dt_prop_header_t read_pheader(const void* addr) {
	buffer_t buff = make_buffer(addr, sizeof(dt_prop_header_t));
	dt_prop_header_t pheader = {0};
	(void)buffer_read_u32_be(buff, 0, &pheader.len);
	(void)buffer_read_u32_be(buff, sizeof(pheader.len), &pheader.nameof);
	return pheader;
}

[[nodiscard]] static inline dt_token_t read_token(const void* dtb_addr) {
	dt_token_t tok = read_be32(dtb_addr);
	return tok;
}

static inline void advance_cursor_at_token(dt_blob_addr_t* cursorOUT) {
	*cursorOUT += sizeof(dt_token_t);
}

static void advance_cursor_at_node(dt_interactive_entity_t* dtie, dt_blob_addr_t* cursorOUT) {
	advance_cursor_at_token(cursorOUT);
	while (*((u8*)dtie->dtb_addr + ++(*cursorOUT)));
	++(*cursorOUT);
	*cursorOUT = align_up_pow2(*cursorOUT, 2);
}

static void advance_cursor_at_prop(dt_interactive_entity_t* dtie, dt_blob_addr_t* cursorOUT) {
	advance_cursor_at_token(cursorOUT);
	dt_prop_header_t pheader = read_pheader(dtie->dtb_addr + *cursorOUT);
	*cursorOUT += sizeof(dt_prop_header_t);
	*cursorOUT += pheader.len;
	*cursorOUT = align_up_pow2(*cursorOUT, 2);
}

[[nodiscard]] static error_t advance_cursor_untill_token_int(dt_interactive_entity_t* dtie, dt_blob_addr_t* cursorOUT, dt_token_t tok,
                                               dt_token_t inttok) {
	bool advanced = false;
	for (u64 limit = 0; limit < 5000; ++limit) {
		dt_token_t token = read_token(dtie->dtb_addr + *cursorOUT);
		if (token == inttok && advanced)
			return ERR_INTERRUPTED;
		if (token == tok && advanced)
			return ERR_NONE;
		switch (token) {
		case DTT_BEGIN_NODE: advance_cursor_at_node(dtie, cursorOUT); break;
		case DTT_END_NODE:   advance_cursor_at_token(cursorOUT); break;
		case DTT_PROP:       advance_cursor_at_prop(dtie, cursorOUT); break;
		case DTT_NOP:        advance_cursor_at_token(cursorOUT); break;
		case DTT_END:        return ERR_NOT_FOUND;
		}
		advanced = true;
	}
	return ERR_NOT_FOUND;
}

[[nodiscard]] error_t push_node(dt_interactive_entity_t* dtie) {
	if(dtie->cursor_stack_pointer == DTIE_STACK_SIZE) return ERR_OUT_OF_BOUNDS;
	dtie->cursor_stack[dtie->cursor_stack_pointer++] = dtie->node_cursor;
	return ERR_NONE;
}

[[nodiscard]] error_t pop_node(dt_interactive_entity_t* dtie) {
	if(dtie->cursor_stack_pointer == 0) return ERR_OUT_OF_BOUNDS;
	dtie->node_cursor = dtie->cursor_stack[dtie->cursor_stack_pointer--];
	return ERR_NONE;
}

/// Public:

error_t dtie_create(const void* dtb_addr, dt_interactive_entity_t* dtieOUT) {
	dt_header_t header = dt_read_header(dtb_addr);
	error_t err = dt_validate_header(header);
	if (err)
		return err;
	dtieOUT->dtb_addr = dtb_addr;
	dtieOUT->header = header;
	dtieOUT->valid = true;
	dtieOUT->prop_cursor = 0;
	dtieOUT->node_cursor = 0;
	dtieOUT->cursor_stack_pointer = 0;
	memset(&dtieOUT->cursor_stack, 0, DTIE_STACK_SIZE * sizeof(dtieOUT->cursor_stack[0]));

	return ERR_NONE;
}

error_t dtie_destroy(dt_interactive_entity_t* dtie) {
	*dtie = (dt_interactive_entity_t){.valid = false};
	return ERR_NONE;
}

error_t dtie_goto_root_node(dt_interactive_entity_t* dtie) {
	if (!dtie->valid)
		return ERR_NOT_VALID;
	dtie->node_cursor = dtie->header.off_dt_struct;
	const void* addr = dtie->dtb_addr + dtie->node_cursor;
	dt_token_t token = read_token(addr);
	if (token != DTT_BEGIN_NODE) {
		dtie->valid = false;
		return ERR_NOT_VALID;
	}
	dtie->prop_cursor = dtie->node_cursor;
	return ERR_NONE;
}

error_t dtie_goto_next_node(dt_interactive_entity_t* dtie) {
	if (!dtie->valid)
		return ERR_NOT_VALID;
	u64 depth = 1;
	error_t err = 0;
	do {
		err = advance_cursor_untill_token_int(dtie, &dtie->node_cursor, DTT_END_NODE, DTT_BEGIN_NODE);
		if(err == ERR_NOT_FOUND) return err;
		if(err == ERR_INTERRUPTED) ++depth;
		else if(err == ERR_NONE) --depth;
	} while(err != ERR_NONE || depth != 0);
	err = advance_cursor_untill_token_int(dtie, &dtie->node_cursor, DTT_BEGIN_NODE, DTT_END_NODE);
	if(err) return ERR_NOT_FOUND;
	dtie->prop_cursor = dtie->node_cursor;
	return ERR_NONE;
}

error_t dtie_goto_child_node(dt_interactive_entity_t* dtie) {
	error_t err = push_node(dtie);
	if(err) return ERR_OUT_OF_BOUNDS;
	err = advance_cursor_untill_token_int(dtie, &dtie->node_cursor, DTT_BEGIN_NODE, DTT_END_NODE);
	dtie->prop_cursor = dtie->node_cursor;
	if(err == ERR_NONE) return err;
	return ERR_NOT_FOUND;
}

error_t dtie_goto_parent_node(dt_interactive_entity_t* dtie) {
	error_t err = pop_node(dtie);
	if (err) return ERR_OUT_OF_BOUNDS;
	dtie->prop_cursor = dtie->node_cursor;
	return ERR_NONE;
}

error_t dtie_goto_next_prop(dt_interactive_entity_t* dtie) {
	if (!dtie->valid)
		return ERR_NOT_VALID;
	error_t err = advance_cursor_untill_token_int(dtie, &dtie->prop_cursor, DTT_PROP, DTT_END_NODE);
	if(err) return ERR_NOT_FOUND;
	return ERR_NONE;
}

error_t dtie_get_node_name(const dt_interactive_entity_t* dtie, buffer_t* buffOUT) {
	if (!dtie->valid)
		return ERR_NOT_VALID;
	const void* addr = dtie->dtb_addr + dtie->node_cursor;
	dt_token_t token = read_token(addr);
	if(token != DTT_BEGIN_NODE) return ERR_BAD_ARG;
	addr += sizeof(dt_token_t);
	u64 strsize = strlen(addr) + 1;
	*buffOUT = make_buffer(addr, strsize);
	return ERR_NONE;
}

error_t dtie_get_prop_name(const dt_interactive_entity_t* dtie, buffer_t* buffOUT) {
	if (!dtie->valid)
		return ERR_NOT_VALID;
	const void* addr = dtie->dtb_addr + dtie->prop_cursor;
	dt_token_t token = read_token(addr);
	if(token != DTT_PROP) return ERR_BAD_ARG;
	addr += sizeof(dt_token_t);
	dt_prop_header_t pheader = read_pheader(addr);
	addr = dtie->dtb_addr + dtie->header.off_dt_strings + pheader.nameof;
	u64 strsize = strlen(addr) + 1;
	*buffOUT = make_buffer(addr, strsize);
	return ERR_NONE;
}

error_t dtie_get_prop_data(const dt_interactive_entity_t* dtie, buffer_t* buffOUT) {
	if (!dtie->valid)
		return ERR_NOT_VALID;
	const void* addr = dtie->dtb_addr + dtie->prop_cursor;
	dt_token_t token = read_token(addr);
	if(token != DTT_PROP) return ERR_BAD_ARG;
	addr += sizeof(dt_token_t);
	dt_prop_header_t pheader = read_pheader(addr);
	addr += sizeof(dt_prop_header_t);
	*buffOUT = make_buffer(addr, pheader.len);
	return ERR_NONE;
}

