#ifndef _STDBIGOS_STRING_H_
#define _STDBIGOS_STRING_H_

#include <stddef.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

[[reproducible, gnu::nonnull]]
void* memcpy(void* restrict dest, const void* restrict src, size_t n);

[[reproducible, gnu::nonnull]]
void* memset(void* dest, int val, size_t n);

[[reproducible, gnu::nonnull]]
void* memset_explicit(void* dest, int val, size_t n);

[[gnu::nonnull]]
void* memmove(void* dest, const void* src, size_t n);

[[reproducible, gnu::nonnull]]
void* memccpy(void* restrict dest, const void* restrict src, int c, size_t count);

[[unsequenced, gnu::nonnull, gnu::pure]]
int memcmp(const void* lhs, const void* rhs, size_t n);

[[unsequenced, gnu::nonnull, gnu::pure]]
void* memchr(const void* ptr, int ch, size_t n);

[[unsequenced, gnu::nonnull, gnu::pure]]
size_t strlen(const char* str);

[[unsequenced, gnu::nonnull, gnu::pure]]
size_t strnlen(const char* str, size_t n);

[[reproducible, gnu::nonnull]]
char* strcpy(char* restrict dest, const char* restrict src);

[[reproducible, gnu::nonnull]]
char* strncpy(char* restrict dest, const char* restrict src, size_t count);

[[gnu::nonnull]]
char* strcat(char* restrict dest, const char* restrict src);

[[gnu::nonnull]]
char* strncat(char* restrict dest, const char* restrict src, size_t count);

[[unsequenced, gnu::nonnull, gnu::pure]]
int strcmp(const char* lhs, const char* rhs);

[[unsequenced, gnu::nonnull, gnu::pure]]
int strncmp(const char* lhs, const char* rhs, size_t n);

[[unsequenced, gnu::nonnull, gnu::pure]]
char* strchr(const char* str, int c);

[[unsequenced, gnu::nonnull, gnu::pure]]
char* strrchr(const char* str, int c);

[[unsequenced, gnu::nonnull, gnu::pure]]
size_t strspn(const char* dest, const char* chars);

[[unsequenced, gnu::nonnull, gnu::pure]]
size_t strcspn(const char* dest, const char* chars);

[[unsequenced, gnu::nonnull, gnu::pure]]
char* strpbrk(const char* dest, const char* breakset);

[[unsequenced, gnu::nonnull, gnu::pure]]
char* strstr(const char* src, const char* dst);

#pragma GCC diagnostic pop

#endif
