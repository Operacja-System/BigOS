#ifndef _STDBIGOS_STRING_H_
#define _STDBIGOS_STRING_H_

#include <stddef.h>

[[gnu::nonnull(1, 2)]] void* memcpy(void* restrict dest,
									const void* restrict src, size_t n);
[[gnu::nonnull(1)]] void* memset(void* dest, int val, size_t n);
[[gnu::nonnull(1)]] void* memset_explicit(void* dest, int val, size_t n);

[[gnu::nonnull(1, 2)]] void* memmove(void* dest, const void* src, size_t n);

[[gnu::nonnull(1), gnu::pure]] size_t strlen(const char* str);
[[gnu::nonnull(1), gnu::pure]] size_t strnlen(const char* str, size_t n);

[[gnu::nonnull(1, 2)]] char* strcpy(char* restrict dest,
									const char* restrict src);
[[gnu::nonnull(1, 2)]] char* strncpy(char* restrict dest,
									 const char* restrict src, size_t count);

[[gnu::nonnull(1, 2)]] char* strcat(char* restrict dest,
									const char* restrict src);
[[gnu::nonnull(1, 2)]] char* strncat(char* restrict dest,
									 const char* restrict src, size_t count);

[[gnu::nonnull(1, 2), gnu::pure]] int memcmp(const void* lhs, const void* rhs,
											 size_t n);
[[gnu::nonnull(1), gnu::pure]] void* memchr(const void* ptr, int ch, size_t n);

[[gnu::nonnull(1, 2), gnu::pure]] int strcmp(const char* lhs, const char* rhs);
[[gnu::nonnull(1, 2), gnu::pure]] int strncmp(const char* lhs, const char* rhs,
											  size_t n);

[[gnu::nonnull(1), gnu::pure]] char* strchr(const char* str, int c);
[[gnu::nonnull(1), gnu::pure]] char* strrchr(const char* str, int c);
[[gnu::nonnull(1, 2), gnu::pure]] size_t strspn(const char* dest,
												const char* chars);
[[gnu::nonnull(1, 2), gnu::pure]] size_t strcspn(const char* dest,
												 const char* chars);
[[gnu::nonnull(1, 2), gnu::pure]] char* strpbrk(const char* dest,
												const char* breakset);
[[gnu::nonnull(1, 2), gnu::pure]] char* strstr(const char* src,
											   const char* dst);
[[gnu::nonnull(1, 2)]] void*
memccpy(void* restrict dest, const void* restrict src, int c, size_t count);

#endif
