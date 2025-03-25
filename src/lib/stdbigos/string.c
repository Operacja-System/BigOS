#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stddef.h>

/* This file contains low level library functions such as memcpy. As such it is
 * optimized for speed. We also define no-tree-loop-distribute-patterns to
 * prevent compiler from "optimize" those functions into a recursive call
 */
#pragma GCC optimize("O3", "no-tree-loop-distribute-patterns")

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
	u8* d = dest;
	const u8* s = src;
	while(n--) { *d++ = *s++; }
	return dest;
}

void* memccpy(void* restrict dest, const void* restrict src, int ch, size_t n) {
	u8* d = dest;
	const u8* s = src;

	while(n--) {
		u8 cur = *s++;
		*d++ = cur;
		if(cur == ch) return d;
	}
	return nullptr;
}

void* memset(void* dest, int val, size_t n) {
	u8* d = dest;
	while(n--) *d++ = val;
	return dest;
}

void* memset_explicit(void* dest, int val, size_t n) {
	return memset(dest, val, n);
}

void* memmove(void* dest, const void* src, size_t n) {
	u8* d = dest;
	const u8* s = src;
	if (d < s) {
		while(n--) *d++ = *s++;
	} else {
		u8 *ld = d + (n-1);
		const u8 *ls = s + (n-1);
		while(n--) *ld-- = *ls--;
	}
	return dest;
}

size_t strlen(const char* str) {
	size_t l = 0;
	while(*str++) l++;
	return l;
}

size_t strnlen(const char* str, size_t n) {
	size_t l = 0;
	while(n-- && *str++) l++;
	return l;
}

char* strcpy(char* restrict dest, const char* restrict src) {
	size_t len = strlen(src);
	return memcpy(dest, src, len + 1);
}

char* strncpy(char* restrict dest, const char* restrict src, size_t count) {
	char* rest = (char*)memccpy(dest, src, '\0', count);
	if(rest)
		memset(rest, '\0', count - (rest - dest));
	else
		dest[count - 1] = '\0'; // non conpliant, but better
	return dest;
}

char* strcat(char* restrict dest, const char* restrict src) {
	strcpy(dest + strlen(dest), src);
	return dest;
}

char* strncat(char* restrict dest, const char* restrict src, size_t n) {
	size_t l = strnlen(dest, n);
	char* end = memccpy(dest + l, src, '\0', n - l);
	if(!end) dest[n] = '\0';
	return dest;
}

int memcmp(const void* lhs_, const void* rhs_, size_t n) {
	const u8* lhs = lhs_;
	const u8* rhs = rhs_;

	while(n--) {
		if(*lhs < *rhs) return -1;
		if(*lhs > *rhs) return 1;
		lhs++;
		rhs++;
	}

	return 0;
}

void* memchr(const void* src_, int ch, size_t n) {
	const u8* s = src_;

	while(n--) {
		if(*s == ch) return (void*)s;
		s++;
	}

	return nullptr;
}

int strcmp(const char* lhs, const char* rhs) {
	while(true) {
		if(*lhs < *rhs) return -1;
		if(*lhs > *rhs) return 1;
		if(!*lhs) return 0;
		lhs++;
		rhs++;
	}
}

int strncmp(const char* lhs, const char* rhs, size_t n) {
	while(n--) {
		if(*lhs < *rhs) return -1;
		if(*lhs > *rhs) return 1;
		if(!*lhs) return 0;
		lhs++;
		rhs++;
	}
	return 0;
}

char* strchr(const char* str, int ch) {
	while(true) {
		if(*str == ch) return (char*)str;
		if(!*str) return nullptr;
		str++;
	}
}

char* strrchr(const char* str, int ch) {
	char* res = nullptr;

	while(*str) {
		if(*str == ch) res = (char*)str;
		str++;
	}

	if('\0' == ch) res = (char*)str;
	return res;
}

char* strstr(const char* str, const char* substr) {
	size_t l1 = strlen(str);
	size_t l2 = strlen(substr);
	if(l2 > l1) return nullptr;

	for(size_t i = 0; i <= l2 - l1; ++i) {
		if(memcmp(str + i, substr, l2) == 0) return (char*)str + i;
	}
	return nullptr;
}

size_t strspn(const char* dest, const char* src) {
	size_t l = 0;
	while(*dest && strchr(src, *dest)) {
		l++;
		dest++;
	}
	return l;
}

size_t strcspn(const char* dest, const char* src) {
	size_t l = 0;
	while(*dest && !strchr(src, *dest)) {
		l++;
		dest++;
	}
	return l;
}

char* strpbrk(const char* dest, const char* breakset) {
	while(*dest) {
		if(strchr(breakset, *dest)) return (char*)dest;
		dest++;
	}
	return nullptr;
}
