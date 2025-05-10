//===-- C standard library header stdfix.h --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_STDFIX_H
#define LLVM_LIBC_STDFIX_H

#include "__llvm-libc-common.h"
#include "llvm-libc-macros/stdfix-macros.h"

// From ISO/IEC TR 18037:2008 standard:
// https://www.iso.org/standard/51126.html
// https://standards.iso.org/ittf/PubliclyAvailableStandards/c051126_ISO_IEC_TR_18037_2008.zip

#include "llvm-libc-macros/stdfix-macros.h"
#include "llvm-libc-types/int_hk_t.h"
#include "llvm-libc-types/int_hr_t.h"
#include "llvm-libc-types/int_k_t.h"
#include "llvm-libc-types/int_lk_t.h"
#include "llvm-libc-types/int_lr_t.h"
#include "llvm-libc-types/int_r_t.h"
#include "llvm-libc-types/stdfix-types.h"
#include "llvm-libc-types/uint_uhk_t.h"
#include "llvm-libc-types/uint_uhr_t.h"
#include "llvm-libc-types/uint_uk_t.h"
#include "llvm-libc-types/uint_ulk_t.h"
#include "llvm-libc-types/uint_ulr_t.h"
#include "llvm-libc-types/uint_ur_t.h"

__BEGIN_C_DECLS

#ifdef LIBC_COMPILER_HAS_FIXED_POINT
short accum abshk(short accum) __NOEXCEPT;

short fract abshr(short fract) __NOEXCEPT;

accum absk(accum) __NOEXCEPT;

long accum abslk(long accum) __NOEXCEPT;

long fract abslr(long fract) __NOEXCEPT;

fract absr(fract) __NOEXCEPT;

int_hk_t bitshk(short accum) __NOEXCEPT;

int_hr_t bitshr(short fract) __NOEXCEPT;

int_k_t bitsk(accum) __NOEXCEPT;

int_lk_t bitslk(long accum) __NOEXCEPT;

int_lr_t bitslr(long fract) __NOEXCEPT;

int_r_t bitsr(fract) __NOEXCEPT;

uint_uhk_t bitsuhk(unsigned short accum) __NOEXCEPT;

uint_uhr_t bitsuhr(unsigned short fract) __NOEXCEPT;

uint_uk_t bitsuk(unsigned accum) __NOEXCEPT;

uint_ulk_t bitsulk(unsigned long accum) __NOEXCEPT;

uint_ulr_t bitsulr(unsigned long fract) __NOEXCEPT;

uint_ur_t bitsur(unsigned fract) __NOEXCEPT;

int countlshk(short accum) __NOEXCEPT;

int countlshr(short fract) __NOEXCEPT;

int countlsk(accum) __NOEXCEPT;

int countlslk(long accum) __NOEXCEPT;

int countlslr(long fract) __NOEXCEPT;

int countlsr(fract) __NOEXCEPT;

int countlsuhk(unsigned short accum) __NOEXCEPT;

int countlsuhr(unsigned short fract) __NOEXCEPT;

int countlsuk(unsigned accum) __NOEXCEPT;

int countlsulk(unsigned long accum) __NOEXCEPT;

int countlsulr(unsigned long fract) __NOEXCEPT;

int countlsur(unsigned fract) __NOEXCEPT;

short accum exphk(short accum) __NOEXCEPT;

accum expk(accum) __NOEXCEPT;

short accum hkbits(int_hk_t) __NOEXCEPT;

short fract hrbits(int_hr_t) __NOEXCEPT;

accum kbits(int_k_t) __NOEXCEPT;

long accum lkbits(uint_ulr_t) __NOEXCEPT;

long fract lrbits(int_lr_t) __NOEXCEPT;

fract rbits(int_r_t) __NOEXCEPT;

short accum roundhk(short accum, int) __NOEXCEPT;

short fract roundhr(short fract, int) __NOEXCEPT;

accum roundk(accum, int) __NOEXCEPT;

long accum roundlk(long accum, int) __NOEXCEPT;

long fract roundlr(long fract, int) __NOEXCEPT;

fract roundr(fract, int) __NOEXCEPT;

unsigned short accum rounduhk(unsigned short accum, int) __NOEXCEPT;

unsigned short fract rounduhr(unsigned short fract, int) __NOEXCEPT;

unsigned accum rounduk(unsigned accum, int) __NOEXCEPT;

unsigned long accum roundulk(unsigned long accum, int) __NOEXCEPT;

unsigned long fract roundulr(unsigned long fract, int) __NOEXCEPT;

unsigned fract roundur(unsigned fract, int) __NOEXCEPT;

unsigned short accum sqrtuhk(unsigned short accum) __NOEXCEPT;

unsigned short fract sqrtuhr(unsigned short fract) __NOEXCEPT;

unsigned accum sqrtuk(unsigned accum) __NOEXCEPT;

unsigned long fract sqrtulr(unsigned long fract) __NOEXCEPT;

unsigned fract sqrtur(unsigned fract) __NOEXCEPT;

unsigned short accum uhkbits(uint_uhk_t) __NOEXCEPT;

unsigned short accum uhksqrtus(unsigned short) __NOEXCEPT;

unsigned short fract uhrbits(uint_uhr_t) __NOEXCEPT;

unsigned accum ukbits(uint_uk_t) __NOEXCEPT;

unsigned accum uksqrtui(unsigned int) __NOEXCEPT;

unsigned long accum ulkbits(uint_ulk_t) __NOEXCEPT;

unsigned long fract ulrbits(uint_ulr_t) __NOEXCEPT;

unsigned fract urbits(uint_ur_t) __NOEXCEPT;
#endif // LIBC_COMPILER_HAS_FIXED_POINT

__END_C_DECLS

#endif // LLVM_LIBC_STDFIX_H
