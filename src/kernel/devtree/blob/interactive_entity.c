#include "interactive_entity.h"

#include <klog.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/string.h>

#include "interactive_entity_internal.h"
#include "stdbigos/buffer.h"

[[nodiscard]] static u64 tokenize_path(char* path) {
	u64 count = 0;
	bool was_prev_fslash = false;
	while (*path) {
		if (*path == '/') {
			was_prev_fslash = true;
			++count;
			*path = '\0';
			++path;
		} else {
			was_prev_fslash = false;
			++path;
		}
	}
	if (!was_prev_fslash)
		++count;
	return count;
}

[[nodiscard]] static const char* get_token(const char* tokpath, u64 tokdepth) {
	const char* ret = tokpath;
	for (u64 i = 0; i < tokdepth; ++i) {
		const char* temp = strchr(ret, '\0');
		if (temp) {
			++temp;
			ret = temp;
		} else {
			return nullptr;
		}
	}
	return ret;
}

[[nodiscard]]
static error_t find_node_at_level(dtb_interactive_entity_t* dtbie, const char* nodename, bool ignore_address) {
	for (u64 i = 0; i < DTBIE_LOOP_LIMIT; ++i) {
		buffer_t buff_nodename = {0};
		error_t err = dtbie_get_node_name(dtbie, &buff_nodename);
		if (err) {
			KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
		}
		if (ignore_address) {
			const char* at_pos = strchr(buff_nodename.data, '@');
			size_t len = at_pos ? (size_t)(at_pos - (char*)buff_nodename.data) : strlen(buff_nodename.data);
			if (strlen(nodename) == len && strncmp(buff_nodename.data, nodename, len) == 0)
				return ERR_NONE;
		} else {
			if (strcmp(buff_nodename.data, nodename) == 0)
				return ERR_NONE;
		}
		err = dtbie_goto_next_node(dtbie);
		if (err)
			return ERR_NOT_FOUND;
	}
	return ERR_NOT_FOUND;
}

[[nodiscard]] static error_t get_cells(dtb_interactive_entity_t* dtbie, u32* addrcellsOUT, u32* sizecellsOUT) {
	error_t err = dtbie_goto_prop(dtbie, "#address-cells");
	if (!err) {
		buffer_t buff = {0};
		err = dtbie_get_prop_data(dtbie, &buff);
		if (err)
			KLOG_RETURN_ERR_TRACE(err);
		err = buffer_read_u32_be(buff, 0, addrcellsOUT);
		if (err)
			KLOG_RETURN_ERR_TRACE(err);
	}
	err = dtbie_goto_prop(dtbie, "#size-cells");
	if (!err) {
		buffer_t buff = {0};
		err = dtbie_get_prop_data(dtbie, &buff);
		if (err)
			KLOG_RETURN_ERR_TRACE(err);
		err = buffer_read_u32_be(buff, 0, addrcellsOUT);
		if (err)
			KLOG_RETURN_ERR_TRACE(err);
	}
	return ERR_NONE;
}

/// Public:

error_t dtbie_create(const void* dtb_addr, dtb_interactive_entity_t* dtbieOUT) {
	dt_header_t header = dt_read_header(dtb_addr);
	error_t err = dt_validate_header(header);
	if (err)
		KLOG_RETURN_ERR_TRACE(err);
	dtbieOUT->dtb_addr = dtb_addr;
	dtbieOUT->header = header;
	dtbieOUT->valid = true;
	dtbieOUT->prop_cursor = 0;
	dtbieOUT->node_cursor = 0;
	dtbieOUT->cursor_stack_pointer = 0;
	memset(&dtbieOUT->cursor_stack, 0, DTBIE_STACK_SIZE * sizeof(dtbieOUT->cursor_stack[0]));

	return ERR_NONE;
}

error_t dtbie_destroy(dtb_interactive_entity_t* dtbie) {
	*dtbie = (dtb_interactive_entity_t){.valid = false};
	return ERR_NONE;
}

error_t dtbie_get_node_cells(dtb_interactive_entity_t* dtbie, u32* addrcellsOUT, u32* sizecellsOUT) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	dtb_addr_t ncur_cpy = dtbie->node_cursor;
	dtb_addr_t pcur_cpy = dtbie->prop_cursor;

	error_t err = 0;
	// default
	*addrcellsOUT = DTB_DEFAULT_ADDRESS_CELLS;
	*sizecellsOUT = DTB_DEFAULT_SIZE_CELLS;
	// parent
	if (dtbie->cursor_stack_pointer != 0) {
		dtbie->node_cursor = dtbie->cursor_stack[dtbie->cursor_stack_pointer - 1];
		err = get_cells(dtbie, addrcellsOUT, sizecellsOUT);
		if (err)
			KLOG_RETURN_ERR_TRACE(err);
	}
	// node
	dtbie->node_cursor = ncur_cpy;
	err = get_cells(dtbie, addrcellsOUT, sizecellsOUT);
	if (err)
		KLOG_RETURN_ERR_TRACE(err);

	dtbie->node_cursor = ncur_cpy;
	dtbie->prop_cursor = pcur_cpy;
	return ERR_NONE;
}

error_t dtbie_goto_node(dtb_interactive_entity_t* dtbie, const char* path) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	size_t strsize = strlen(path);
	char tokpath[strsize + 1];
	strcpy(tokpath, path);
	tokpath[strsize] = '\0';
	const char* tokpath_ptr = tokpath;
	u64 depth = tokenize_path(tokpath);

	error_t err = 0;
	if (path[0] == '/') {
		err = dtbie_goto_root_node(dtbie);
	} else {
		err = dtbie_goto_child_node(dtbie);
	}
	if (err == ERR_NOT_FOUND)
		return err;
	if (err)
		KLOG_RETURN_ERR_TRACE(err);

	for (u64 i = 0; i < depth - 1; ++i) {
		err = find_node_at_level(dtbie, tokpath_ptr, false);
		if (err)
			KLOG_RETURN_ERR_TRACE(err);
		err = dtbie_goto_child_node(dtbie);
		if (err)
			KLOG_RETURN_ERR_TRACE(err);
		size_t strsize = strlen(tokpath_ptr);
		tokpath_ptr += strsize + 1;
	}
	err = find_node_at_level(dtbie, tokpath_ptr, false);
	if (err)
		KLOG_RETURN_ERR_TRACE(err);
	return ERR_NONE;
}

error_t dtbie_goto_node_ignore_address(dtb_interactive_entity_t* dtbie, const char* path, u64 nodenum) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	size_t strsize = strlen(path);
	char tokpath[strsize + 1];
	strcpy(tokpath, path);
	tokpath[strsize] = '\0';
	const char* tokpath_ptr = tokpath;
	u64 depth = tokenize_path(tokpath);

	error_t err = 0;
	if (path[0] == '/') {
		err = dtbie_goto_root_node(dtbie);
	} else {
		err = dtbie_goto_child_node(dtbie);
	}
	if (err == ERR_NOT_FOUND)
		return err;
	if (err)
		KLOG_RETURN_ERR_TRACE(err);

	for (u64 i = 0; i < depth - 1; ++i) {
		err = find_node_at_level(dtbie, tokpath_ptr, true);
		if (err)
			KLOG_RETURN_ERR_TRACE(err);
		err = dtbie_goto_child_node(dtbie);
		if (err)
			KLOG_RETURN_ERR_TRACE(err);
		size_t strsize = strlen(tokpath_ptr);
		tokpath_ptr += strsize + 1;
	}
	for (u64 i = 0; i <= nodenum; ++i) {
		err = find_node_at_level(dtbie, tokpath_ptr, true);
		if (err == ERR_NOT_FOUND)
			return err;
		if (err)
			KLOG_RETURN_ERR_TRACE(err);
		if (i < nodenum) {
			err = dtbie_goto_next_node(dtbie);
			if (err)
				KLOG_RETURN_ERR_TRACE(err);
		}
	}
	return ERR_NONE;
}

error_t dtbie_count_children_nodes(dtb_interactive_entity_t* dtbie, const char* nodename, u64* countOUT) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	dtb_addr_t prop_cursor_cpy = dtbie->prop_cursor;
	error_t err = dtbie_goto_child_node(dtbie);
	*countOUT = 0;
	for (u64 i = 0; i < DTBIE_LOOP_LIMIT; ++i) {
		err = find_node_at_level(dtbie, nodename, true);
		if (err)
			break;
		++(*countOUT);
		err = dtbie_goto_next_node(dtbie);
		if (err)
			break;
	}
	err = dtbie_goto_parent_node(dtbie);
	if (err)
		KLOG_RETURN_ERR_TRACE(err);
	dtbie->prop_cursor = prop_cursor_cpy;
	return ERR_NONE;
}

error_t dtbie_goto_prop(dtb_interactive_entity_t* dtbie, const char* propname) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	dtbie->prop_cursor = dtbie->node_cursor;
	error_t err = dtbie_goto_next_prop(dtbie);
	if (err)
		return ERR_NOT_FOUND;
	for (u64 i = 0; i < DTBIE_LOOP_LIMIT; ++i) {
		buffer_t buff_propname = {0};
		err = dtbie_get_prop_name(dtbie, &buff_propname);
		if (err)
			KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
		if (strcmp(buff_propname.data, propname) == 0) {
			return ERR_NONE;
		}
		err = dtbie_goto_next_prop(dtbie);
		if (err)
			return ERR_NOT_FOUND;
	}
	return ERR_NOT_FOUND;
}

error_t dtbie_does_node_exist(dtb_interactive_entity_t* dtbie, const char* path) {
	if (!dtbie->valid)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	error_t err = dtbie_goto_node(dtbie, path);
	return err;
}

