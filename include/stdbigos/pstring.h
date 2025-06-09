#ifndef STDBIGOS_PSTRING_H
#define STDBIGOS_PSTRING_H

#include <stdbigos/meta/err_x_t.h>
#include <stdbigos/types.h>

/// Pascal-like length based string
typedef struct {
	size_t len;
	char* data;
} pstring_t;

ERRX_WRAPPER_T(pstring, pstring_t);
ERRX_WRAPPER_T(size, size_t);
ERRX_WRAPPER_T(int, int);

/// Wrap null terminated string in `pstring_t` stuct
[[nodiscard]]
pstring_or_err_t pstring_l2w(char* str);

/// Returns the length of the pstring_t.
[[nodiscard]]
size_or_err_t pstring_len(const pstring_t* ps);

/// Returns the length of the pstring_t.
/// If ps is `NULL` or does not have `data` then `0` will be returned
[[nodiscard]]
size_t pstring_len_silenterr(const pstring_t* ps);

// The following functions are more-or-less analogous to their [xxx]n[yyy]
// string.h counterparts but check correctness of the pstring_t argument.

[[nodiscard]]
size_or_err_t pstring_memcpy(pstring_t* dest, const pstring_t* src);

/// Fills the `ps` with `val` value
/// Depends on memory being already allocated
[[nodiscard]]
error_t pstring_fill(pstring_t* ps, char val);

/// Copy `count` bytes from `src` to `dest`. Bounds are checked.
/// Returns the number of bytes copied.
[[nodiscard]]
size_or_err_t pstring_memmove(pstring_t* dest, const pstring_t* src, size_t count);

[[nodiscard]]
int_or_err_t pstring_strcmp(const pstring_t* lhs, const pstring_t* rhs);

[[nodiscard]]
pstring_or_err_t pstring_strchr(const pstring_t* ps, char ch);

/// Returns a slice view for a given string.
/// Characters of slice view represent range `[start, end)`
[[nodiscard]]
pstring_or_err_t pstring_slice_view(const pstring_t* ps, u32 start, u32 end);

#endif
