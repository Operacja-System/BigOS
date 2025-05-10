//===-- C standard library header time.h ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TIME_H
#define LLVM_LIBC_TIME_H

#include "__llvm-libc-common.h"
#include "llvm-libc-macros/time-macros.h"

#include "llvm-libc-types/clock_t.h"
#include "llvm-libc-types/clockid_t.h"
#include "llvm-libc-types/locale_t.h"
#include "llvm-libc-types/size_t.h"
#include "llvm-libc-types/struct_timespec.h"
#include "llvm-libc-types/struct_timeval.h"
#include "llvm-libc-types/struct_tm.h"
#include "llvm-libc-types/time_t.h"

__BEGIN_C_DECLS

char *asctime(const struct tm *) __NOEXCEPT;

char *asctime_r(struct tm *, char *) __NOEXCEPT;

char *ctime(const time_t *) __NOEXCEPT;

char *ctime_r(const time_t *, char *) __NOEXCEPT;

double difftime(time_t, time_t) __NOEXCEPT;

struct tm *gmtime(time_t *) __NOEXCEPT;

struct tm *gmtime_r(time_t *, struct tm *) __NOEXCEPT;

time_t mktime(struct tm *) __NOEXCEPT;

size_t strftime(char *__restrict, size_t, const char *__restrict, const struct tm *__restrict) __NOEXCEPT;

size_t strftime_l(char *__restrict, size_t, const char *__restrict, const struct tm *__restrict, locale_t) __NOEXCEPT;

int timespec_get(struct timespec *, int) __NOEXCEPT;

__END_C_DECLS

#endif // LLVM_LIBC_TIME_H
