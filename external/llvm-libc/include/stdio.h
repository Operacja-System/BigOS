//===-- C standard library header stdio.h ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_STDIO_H
#define LLVM_LIBC_STDIO_H

#include "__llvm-libc-common.h"
#include "llvm-libc-macros/file-seek-macros.h"
#include "llvm-libc-macros/stdio-macros.h"

#include <stdarg.h>

#include "llvm-libc-types/FILE.h"
#include "llvm-libc-types/cookie_io_functions_t.h"
#include "llvm-libc-types/off_t.h"
#include "llvm-libc-types/size_t.h"
#include <stdarg.h>

#define stdout stdout

#define stdin stdin

#define stderr stderr

__BEGIN_C_DECLS

int asprintf(char **__restrict, const char *__restrict, ...) __NOEXCEPT;

int getchar(void) __NOEXCEPT;

int printf(const char *__restrict, ...) __NOEXCEPT;

int putchar(int) __NOEXCEPT;

int puts(const char *) __NOEXCEPT;

int remove(const char *) __NOEXCEPT;

int scanf(const char *__restrict, ...) __NOEXCEPT;

int snprintf(char *__restrict, size_t, const char *__restrict, ...) __NOEXCEPT;

int sprintf(char *__restrict, const char *__restrict, ...) __NOEXCEPT;

int sscanf(const char *__restrict, const char *__restrict, ...) __NOEXCEPT;

int vasprintf(char **__restrict, const char *__restrict, va_list) __NOEXCEPT;

int vprintf(const char *__restrict, va_list) __NOEXCEPT;

int vscanf(const char *__restrict, va_list) __NOEXCEPT;

int vsnprintf(char *__restrict, size_t, const char *__restrict, va_list) __NOEXCEPT;

int vsprintf(char *__restrict, const char *__restrict, va_list) __NOEXCEPT;

int vsscanf(const char *__restrict, const char *__restrict, va_list) __NOEXCEPT;

extern FILE * stderr;
extern FILE * stdin;
extern FILE * stdout;

__END_C_DECLS

#endif // LLVM_LIBC_STDIO_H
