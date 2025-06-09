#include <stdarg.h>
#include <stdbigos/pstring.h>
#include <stdbigos/string.h>
#include <stddef.h>

#define ASSERT_PS_OR_ERR_T(tp, x)                 \
	if ((x) == nullptr || (x)->data == nullptr) { \
		return (tp){.err = ERR_INVALID_ARGUMENT}; \
	}
#define ASSERT_PS_OR_ERR(x)                       \
	if ((x) == nullptr || (x)->data == nullptr) { \
		return ERR_INVALID_ARGUMENT;              \
	}

pstring_or_err_t pstring_l2w(char* str) {
	if (str == nullptr) {
		return (pstring_or_err_t){.err = ERR_INVALID_ARGUMENT};
	}
	return (pstring_or_err_t){
	    .val = {.len = strlen(str), .data = str}
    };
}

size_or_err_t pstring_len(const pstring_t* ps) {
	ASSERT_PS_OR_ERR_T(size_or_err_t, ps);
	return (size_or_err_t){.val = ps->len};
}

size_t pstring_len_silenterr(const pstring_t* ps) {
	return ERRX_UNWRAP_OR_DEFAULT(pstring_len(ps), 0);
}

size_or_err_t pstring_memcpy(pstring_t* dest, const pstring_t* src) {
	ASSERT_PS_OR_ERR_T(size_or_err_t, dest);
	ASSERT_PS_OR_ERR_T(size_or_err_t, src);
	const u32 dest_len = pstring_len_silenterr(dest);
	const u32 src_len = pstring_len_silenterr(src);
	if (dest_len == 0 || src_len == 0) {
		return (size_or_err_t){.val = 0};
	}

	const u32 min_len = (dest_len < src_len) ? dest_len : src_len;
	dest->data = (char*)memcpy(dest->data, src->data, min_len);
	return (size_or_err_t){.val = 0};
}

error_t pstring_fill(pstring_t* ps, char val) {
	ASSERT_PS_OR_ERR(ps);

	const u32 len = pstring_len_silenterr(ps);

	for (u32 i = 0; i < len; i++) {
		ps->data[i] = val;
	}

	return len;
}

size_or_err_t pstring_memmove(pstring_t* dest, const pstring_t* src, size_t count) {
	ASSERT_PS_OR_ERR_T(size_or_err_t, dest);
	ASSERT_PS_OR_ERR_T(size_or_err_t, src);
	const u32 dest_len = pstring_len_silenterr(dest);
	const u32 src_len = pstring_len_silenterr(src);
	if (dest_len == 0 || src_len == 0) {
		return (size_or_err_t){.val = 0};
	}

	const u32 min_len = (src->len < dest->len) ? src->len : dest->len;
	count = (min_len < count) ? min_len : count;

	dest->data = (char*)memmove(dest->data, src->data, count);
	return (size_or_err_t){.val = count};
}

int_or_err_t pstring_cmp(const pstring_t* lhs, const pstring_t* rhs) {
	ASSERT_PS_OR_ERR_T(int_or_err_t, lhs);
	ASSERT_PS_OR_ERR_T(int_or_err_t, rhs);
	const u32 lhs_len = pstring_len_silenterr(lhs);
	const u32 rhs_len = pstring_len_silenterr(rhs);
	if (lhs_len != rhs_len) {
		return (int_or_err_t){.val = lhs_len < rhs_len ? -1 : 1};
	}

	size_t l = 0;
	size_t r = 0;
	while (l < lhs_len && r < rhs_len) {
		int cmp = lhs->data[l] - rhs->data[r];
		if (cmp != 0) {
			return (int_or_err_t){.val = cmp};
		}

		l++;
		r++;
	}

	return (int_or_err_t){.val = 0};
}

pstring_or_err_t pstring_strchr(const pstring_t* ps, char ch) {
	ASSERT_PS_OR_ERR_T(pstring_or_err_t, ps);
	size_t len = ps->len;
	for (size_t i = 0; i < ps->len; i++, len--) {
		if (ps->data[i] == ch) {
			return (pstring_or_err_t){
			    .val = {.len = len, .data = ps->data + i}
            };
		}
	}

	return (pstring_or_err_t){
	    .val = {.len = 0, .data = nullptr}
    };
}

pstring_or_err_t pstring_slice_view(const pstring_t* ps, u32 start, u32 end) {
	ASSERT_PS_OR_ERR_T(pstring_or_err_t, ps);
	const size_t ps_len = pstring_len_silenterr(ps);
	if (ps_len == 0 || ps_len < start) {
		return (pstring_or_err_t){
		    .val = {.len = 0, .data = nullptr}
        };
	}

	end = (ps_len < end) ? ps_len : end;
	const u32 new_len = end - start;
	return (pstring_or_err_t){
	    .val = {.len = new_len, .data = ps->data + start}
    };
}

size_or_err_t pstring_cat(pstring_t* src1, size_t from, const pstring_t* src2) {
	ASSERT_PS_OR_ERR_T(size_or_err_t, src1);
	ASSERT_PS_OR_ERR_T(size_or_err_t, src2);

	size_t num_written = 0;
	size_t i = from;
	while (i < src1->len && num_written < src2->len) {
		src1->data[i++] = src2->data[num_written++];
	}

	return (size_or_err_t){.val = num_written};
}

size_or_err_t pstring_concat(pstring_t* dest, const pstring_t* first, const pstring_t* second) {
	ASSERT_PS_OR_ERR_T(size_or_err_t, dest);
	ASSERT_PS_OR_ERR_T(size_or_err_t, first);
	ASSERT_PS_OR_ERR_T(size_or_err_t, second);

	size_t num_copied = 0;
	for (size_t i = 0; i < first->len && num_copied < dest->len; i++, num_copied++) {
		dest->data[num_copied] = first->data[i];
	}
	for (size_t i = 0; i < second->len && num_copied < dest->len; i++, num_copied++) {
		dest->data[num_copied] = second->data[i];
	}

	return (size_or_err_t){.val = num_copied};
}
