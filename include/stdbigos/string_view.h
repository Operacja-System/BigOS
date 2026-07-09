#ifndef STDBIGOS_STRING_VIEW
#define STDBIGOS_STRING_VIEW

#include <stddef.h>
#include <stdbigos/math.h>
#include <stdbigos/string.h>

typedef struct {
	const char* data;
	size_t count;
} string_view;

static inline string_view string_view_init(const char* str) {
	if (!str)
		return (string_view){nullptr, 0};
	return (string_view){str, strlen(str)};
}

static inline string_view string_view_pop_front(string_view strv) {
	if (strv.count == 0)
		return strv;
	strv.data++;
	strv.count--;
	return strv;
}

static inline string_view string_view_pop_back(string_view strv) {
	if (strv.count == 0)
		return strv;
	strv.count--;
	return strv;
}

static inline int string_view_cmp(string_view a, string_view b) {
	size_t min = MIN(a.count, b.count);

	for (size_t i = 0; i < min; i++) {
		unsigned char ca = (unsigned char)a.data[i];
		unsigned char cb = (unsigned char)b.data[i];

		if (ca < cb)
			return -1;
		if (ca > cb)
			return 1;
	}

	if (a.count < b.count)
		return -1;
	if (a.count > b.count)
		return 1;
	return 0;
}

static inline int string_view_ncmp(string_view a, string_view b, size_t n) {
	size_t min = MIN(MIN(a.count, b.count), n);

	for (size_t i = 0; i < min; i++) {
		unsigned char ca = (unsigned char)a.data[i];
		unsigned char cb = (unsigned char)b.data[i];

		if (ca < cb)
			return -1;
		if (ca > cb)
			return 1;
	}

	if (a.count < n && a.count < b.count)
		return -1;
	if (b.count < n && b.count < a.count)
		return 1;
	return 0;
}

static inline string_view string_view_chr(string_view strv, int c) {
	if (!strv.data)
		return (string_view){NULL, 0};
	unsigned char ch = (unsigned char)c;
	while (strv.count > 0 && (unsigned char)strv.data[0] != ch) {
		strv = string_view_pop_front(strv);
	}
	return strv;
}

static inline string_view string_view_tokenize(string_view strv, string_view delim) {
	if (!strv.data)
		return (string_view){NULL, 0};
	string_view ret;
	ret.data = strv.data;
	ret.count = 0;
	while (strv.count > 0 && string_view_chr(delim, *strv.data).count == 0) {
		strv = string_view_pop_front(strv);
		ret.count++;
	}
	return ret;
}

static inline size_t string_view_spn(string_view strv, string_view chars) {
	if (!strv.data)
		return 0;
	for (size_t i = 0; i < strv.count; i++) {
		if (string_view_chr(chars, strv.data[i]).count == 0)
			return i;
	}
	return strv.count;
}

static inline size_t string_view_cspn(string_view strv, string_view chars) {
	if (!strv.data)
		return 0;
	for (size_t i = 0; i < strv.count; i++) {
		if (string_view_chr(chars, strv.data[i]).count > 0)
			return i;
	}
	return strv.count;
}

static inline string_view string_view_pbrk(string_view strv, string_view breakset) {
	if (!strv.data)
		return (string_view){NULL, 0};
	while (strv.count > 0 && string_view_chr(breakset, strv.data[0]).count == 0) {
		strv = string_view_pop_front(strv);
	}
	return strv;
}

static inline string_view string_view_str(string_view strv, string_view substrv) {
	if (!strv.data)
		return (string_view){NULL, 0};
	if (substrv.count == 0)
		return (string_view){strv.data, 0};
	if (!substrv.data || strv.count < substrv.count)
		return (string_view){NULL, 0};
	size_t count = strv.count;
	strv.count = substrv.count;
	for (size_t i = 0; i <= count - substrv.count; i++) {
		if (string_view_cmp(strv, substrv) == 0)
			return strv;
		strv.data++;
	}
	return (string_view){NULL, 0};
}

#endif // !STDBIGOS_STRING_VIEW
