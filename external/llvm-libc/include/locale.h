//===-- C standard library header locale.h --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_LOCALE_H
#define LLVM_LIBC_LOCALE_H

#include "__llvm-libc-common.h"

#include "llvm-libc-macros/locale-macros.h"

#include "llvm-libc-types/locale_t.h"
#include "llvm-libc-types/struct_lconv.h"

__BEGIN_C_DECLS

locale_t duplocale(locale_t) __NOEXCEPT;

void freelocale(locale_t) __NOEXCEPT;

struct lconv *localeconv(void) __NOEXCEPT;

locale_t newlocale(int, const char *, locale_t) __NOEXCEPT;

char *setlocale(int, const char *) __NOEXCEPT;

locale_t uselocale(locale_t) __NOEXCEPT;

__END_C_DECLS

#endif // LLVM_LIBC_LOCALE_H
