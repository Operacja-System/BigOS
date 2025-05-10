//===-- C standard library header strings.h -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_STRINGS_H
#define LLVM_LIBC_STRINGS_H

#include "__llvm-libc-common.h"

#include "llvm-libc-types/locale_t.h"
#include "llvm-libc-types/size_t.h"

__BEGIN_C_DECLS

int bcmp(const void *, const void *, size_t) __NOEXCEPT;

void bcopy(const void *, void *, size_t) __NOEXCEPT;

void bzero(void *, size_t) __NOEXCEPT;

int ffs(int) __NOEXCEPT;

int ffsl(long) __NOEXCEPT;

int ffsll(long long) __NOEXCEPT;

char *index(const char *, int) __NOEXCEPT;

char *rindex(const char *, int) __NOEXCEPT;

int strcasecmp(const char *, const char *) __NOEXCEPT;

int strcasecmp_l(const char *, const char *, locale_t) __NOEXCEPT;

int strncasecmp(const char *, const char *, size_t) __NOEXCEPT;

int strncasecmp_l(const char *, const char *, size_t, locale_t) __NOEXCEPT;

__END_C_DECLS

#endif // LLVM_LIBC_STRINGS_H
