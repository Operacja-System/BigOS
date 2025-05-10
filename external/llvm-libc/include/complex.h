//===-- C standard library header complex.h -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_COMPLEX_H
#define LLVM_LIBC_COMPLEX_H

#include "__llvm-libc-common.h"
#include "llvm-libc-macros/complex-macros.h"

#include "llvm-libc-types/cfloat128.h"
#include "llvm-libc-types/cfloat16.h"
#include "llvm-libc-types/float128.h"

__BEGIN_C_DECLS

double cimag(_Complex double) __NOEXCEPT;

float cimagf(_Complex float) __NOEXCEPT;

long double cimagl(_Complex long double) __NOEXCEPT;

_Complex double conj(_Complex double) __NOEXCEPT;

_Complex float conjf(_Complex float) __NOEXCEPT;

_Complex long double conjl(_Complex long double) __NOEXCEPT;

_Complex double cproj(_Complex double) __NOEXCEPT;

_Complex float cprojf(_Complex float) __NOEXCEPT;

_Complex long double cprojl(_Complex long double) __NOEXCEPT;

double creal(_Complex double) __NOEXCEPT;

float crealf(_Complex float) __NOEXCEPT;

long double creall(_Complex long double) __NOEXCEPT;

__END_C_DECLS

#endif // LLVM_LIBC_COMPLEX_H
