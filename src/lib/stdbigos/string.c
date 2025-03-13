#include "string.h"

#include <stddef.h>

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
	char* d = dest;
	const char* s = src;
	while(n--) { *d++ = *s++; }
	return dest;
}

void* memccpy(void* restrict dest, const void* restrict src, int ch, size_t n) {
	unsigned char* d = dest;
	const unsigned char* s = src;

	while(n--) {
		unsigned char cur = *s++;
		*d++ = cur;
		if (cur == (unsigned char)ch) return d;
	}
	return nullptr;
}

void* memset(void* dest, int val, size_t n) {
	char* d = dest;
	while(--n) *d++ = (char)val;
	return dest;
}

void* memset_explicit(void* dest, int val, size_t n) {
	return memset(dest, val, n);
}

void* memmove(void* dest, const void* src, size_t n) {
	char *d = dest;
	const char* s = src;
	while(n--) { *d++ = *s++; }
	return dest;
}

size_t strlen(const char* str) {
	size_t l = 0;
	while(*str++) l++;
	return l;
}

size_t strnlen(const char* str, size_t n) {
	size_t l = 0;
	while (--n && *str++) l++;
	return l;
}

char* strcpy(char* restrict dest, const char* restrict src) {
	size_t len = strlen(src);
	return memcpy(dest, src, len + 1);
}

char* strncpy(char* restrict dest, const char* restrict src, size_t count) {
	size_t len = strlen(src);
	if(len + 1 <= count) {
		return memcpy(dest, src, len + 1);
	} else {
		memcpy(dest, src, count);
		dest[count - 1] = '\0';
		return dest;
	}
}

char* strcat(char* restrict dest, const char* restrict src) {
	strcpy(dest + strlen(dest), src);
	return dest;
}

char* strncat(char* restrict dest, const char* restrict src, size_t n) {
	size_t l = strnlen(dest, n);
	strncpy(dest + l, src, n - l);
	return dest;
}

int memcmp(const void* lhs_, const void* rhs_, size_t n) {
	const unsigned char* lhs = lhs_;
	const unsigned char* rhs = rhs_;

	while (n--) {
		if (*lhs < *rhs) return -1;
		if (*lhs > *rhs) return 1;
	}

	return 0;
}

void* memchr(const void* src_, int ch, size_t n) {
	const unsigned char *s = src_;
	unsigned char c = ch;

	while (n--) {
		if (*s == c) return (void*)s;
		s++;
	}

	return nullptr;
}

int strcmp(const char* lhs, const char* rhs) {
	size_t l1 = strlen(lhs);
	size_t l2 = strlen(rhs);

	size_t mi = l1 < l2 ? l1 : l2;

	int r = memcmp(lhs, rhs, mi);
	if (r != 0) return r;

	if (l1 == l2) return 0;
	return l1 < l2 ? -1 : 1;
}

int strncmp(const char* lhs, const char* rhs, size_t n) {
	size_t l1 = strnlen(lhs, n);
	size_t l2 = strnlen(rhs, n);

	size_t mi = l1 < l2 ? l1 : l2;

	int r = memcmp(lhs, rhs, mi);
	if (r != 0) return r;

	if (l1 == l2) return 0;
	return l1 < l2 ? -1 : 1;
}

char* strchr(const char *str, int ch) {
	size_t l = strlen(str);
	return memchr(str, ch, l+1);
}

char* strrchr(const char* str, int ch) {
	char *res = nullptr;

	while (*str) {
		if (*str == ch) res = (char*)str;
		str++;
	}

	if ('\0' == ch) res = (char*)str;
	return res;
}

char* strstr(const char *str, const char *substr) {
	size_t l1 = strlen(str);
	size_t l2 = strlen(substr);
	if (l2 > l1) return nullptr;

	for (size_t i = 0; i <= l2 - l1; ++i) {
		if (memcmp(str + i, substr, l2) == 0)
			return (char*)str + i;
	}
	return nullptr;
}

size_t strspn(const char *dest, const char *src) {
	size_t l = 0;
	while (*dest && strchr(src, *dest)) {
		l++;
		dest++;
	}
	return l;
}

size_t strcspn(const char *dest, const char *src) {
	size_t l = 0;
	while (*dest && !strchr(src, *dest)) {
		l++;
		dest++;
	}
	return l;
}

char *strpbrk( const char *dest, const char *breakset ) {
	while (*dest) {
		if (strchr(breakset, *dest)) return (char*)dest;
		dest++;
	}
	return nullptr;
}
