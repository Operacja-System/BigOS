//===-- Standard C header <ctype.h> --===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//

#ifndef _LLVM_LIBC_CTYPE_H
#define _LLVM_LIBC_CTYPE_H

#include "__llvm-libc-common.h"
#include "llvm-libc-types/locale_t.h"

__BEGIN_C_DECLS

int isalnum(int) __NOEXCEPT;

int isalnum_l(int, locale_t) __NOEXCEPT;

int isalpha(int) __NOEXCEPT;

int isalpha_l(int, locale_t) __NOEXCEPT;

int isascii(int) __NOEXCEPT;

int isblank(int) __NOEXCEPT;

int isblank_l(int, locale_t) __NOEXCEPT;

int iscntrl(int) __NOEXCEPT;

int iscntrl_l(int, locale_t) __NOEXCEPT;

int isdigit(int) __NOEXCEPT;

int isdigit_l(int, locale_t) __NOEXCEPT;

int isgraph(int) __NOEXCEPT;

int isgraph_l(int, locale_t) __NOEXCEPT;

int islower(int) __NOEXCEPT;

int islower_l(int, locale_t) __NOEXCEPT;

int isprint(int) __NOEXCEPT;

int isprint_l(int, locale_t) __NOEXCEPT;

int ispunct(int) __NOEXCEPT;

int ispunct_l(int, locale_t) __NOEXCEPT;

int isspace(int) __NOEXCEPT;

int isspace_l(int, locale_t) __NOEXCEPT;

int isupper(int) __NOEXCEPT;

int isupper_l(int, locale_t) __NOEXCEPT;

int isxdigit(int) __NOEXCEPT;

int isxdigit_l(int, locale_t) __NOEXCEPT;

int toascii(int) __NOEXCEPT;

int tolower(int) __NOEXCEPT;

int tolower_l(int, locale_t) __NOEXCEPT;

int toupper(int) __NOEXCEPT;

int toupper_l(int, locale_t) __NOEXCEPT;

__END_C_DECLS

#endif // _LLVM_LIBC_CTYPE_H
