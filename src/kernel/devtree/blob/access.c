#include "access.h"

#include <stdbigos/string.h>
#include <klog.h>

#include "interactive_entity.h"
#include "stdbigos/buffer.h"
#include "stdbigos/error.h"

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

[[nodiscard]] error_t find_node_at_level(dt_interactive_entity_t* dtie, const char* nodename, bool ignore_address) {
	for (u64 i = 0; i < 5000; ++i) {
		buffer_t buff_nodename = {0};
		error_t err = dtie_get_node_name(dtie, &buff_nodename);
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
		err = dtie_goto_next_node(dtie);
		if (err)
			return ERR_NOT_FOUND;
	}
	return ERR_NOT_FOUND;
}

[[nodiscard]] error_t find_prop_at_level(dt_interactive_entity_t* dtie, const char* propname, buffer_t* buffOUT) {
	for (u64 i = 0; i < 5000; ++i) {
		buffer_t buff_propname = {0};
		error_t err = dtie_get_prop_name(dtie, &buff_propname);
		if (err) 
			KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
		if (strcmp(buff_propname.data, propname) == 0) {
			err = dtie_get_prop_data(dtie, buffOUT);
			if (err)
				KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
			return ERR_NONE;
		}
		err = dtie_goto_next_prop(dtie);
		if (err)
			return ERR_NOT_FOUND;
	}
	return ERR_NOT_FOUND;
}

[[nodiscard]] error_t goto_node_path(dt_interactive_entity_t* dtie, const char* path, bool ignore_address) {
	u64 strsize = strlen(path);
	char path_cpy[strsize + 1];
	strcpy(path_cpy, path);
	path_cpy[strsize] = '\0';
	const char* path_ptr = path_cpy;
	u64 depth = tokenize_path(path_cpy);
	error_t err = 0;
	for (u64 i = 0; i < depth - 1; ++i) {
		err = find_node_at_level(dtie, path_ptr, ignore_address);
		if (err)
			KLOG_RETURN_ERR_TRACE(err);
		err = dtie_goto_child_node(dtie);
		if (err)
			KLOG_RETURN_ERR_TRACE(err);
		strsize = strlen(path_ptr);
		path_ptr += strsize + 1;
	}
	err = find_node_at_level(dtie, path_ptr, ignore_address);
	if (err)
		KLOG_RETURN_ERR_TRACE(err);
	return ERR_NONE;
}

/// Public:

error_t dtb_get_prop_data(const void* dtb, const char* path, const char* prop_name, bool ignore_address,
                          buffer_t* buffOUT) {
	if (!dtb || !path || !prop_name || !buffOUT)
		KLOG_RETURN_ERR_TRACE(ERR_BAD_ARG);
	dt_interactive_entity_t dtie = {0};
	error_t err = dtie_create(dtb, &dtie);
	if (err)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	err = dtie_goto_root_node(&dtie);
	if (err)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	err = goto_node_path(&dtie, path, ignore_address);
	if (err)
		KLOG_RETURN_ERR_TRACE(err);
	err = dtie_goto_next_prop(&dtie);
	if (err)
		KLOG_RETURN_ERR_TRACE(err);
	err = find_prop_at_level(&dtie, prop_name, buffOUT);
	if (err)
		KLOG_RETURN_ERR_TRACE(err);
	return ERR_NONE;
}

error_t dtb_does_node_exist(const void* dtb, const char* path, bool ignore_address) {
	if (!dtb || !path)
		KLOG_RETURN_ERR_TRACE(ERR_BAD_ARG);
	dt_interactive_entity_t dtie = {0};
	error_t err = dtie_create(dtb, &dtie);
	if (err)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	err = dtie_goto_root_node(&dtie);
	if (err)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	err = goto_node_path(&dtie, path, ignore_address);
	if (err)
		return ERR_NOT_FOUND;
	return ERR_NONE;
}

error_t dtb_count_nodes_ignore_address(const void* dtb, const char* path, const char* name, u64* cOUT) {
	if (!dtb || !path)
		KLOG_RETURN_ERR_TRACE(ERR_BAD_ARG);
	dt_interactive_entity_t dtie = {0};
	error_t err = dtie_create(dtb, &dtie);
	if (err)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	err = dtie_goto_root_node(&dtie);
	if (err)
		KLOG_RETURN_ERR_TRACE(ERR_NOT_VALID);
	*cOUT = 0;

	err = goto_node_path(&dtie, path, false);
	if (err)
		return ERR_NOT_FOUND;
	buffer_t buf1 = {0};
	(void)dtie_get_node_name(&dtie, &buf1);
	err = dtie_goto_child_node(&dtie);
	if (err)
		return ERR_NOT_FOUND;

	for (u64 i = 0; i < 5000; ++i) {
		err = find_node_at_level(&dtie, name, true);
		if (err)
			return ERR_NONE;
		++(*cOUT);
		err = dtie_goto_next_node(&dtie);
		if (err)
			return ERR_NONE;
	}
	KLOG_RETURN_ERR_TRACE(ERR_INTERNAL_FAILURE); //This should never happen
}

