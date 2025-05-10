//===-- C standard library header math.h ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MATH_H
#define LLVM_LIBC_MATH_H

#include "__llvm-libc-common.h"
#include "llvm-libc-macros/float16-macros.h"
#include "llvm-libc-macros/math-macros.h"


#include "llvm-libc-types/double_t.h"
#include "llvm-libc-types/float128.h"
#include "llvm-libc-types/float_t.h"

__BEGIN_C_DECLS

float acosf(float) __NOEXCEPT;

float asinf(float) __NOEXCEPT;

float asinhf(float) __NOEXCEPT;

double atan(double) __NOEXCEPT;

double atan2(double, double) __NOEXCEPT;

float atan2f(float, float) __NOEXCEPT;

float atanf(float) __NOEXCEPT;

float atanhf(float) __NOEXCEPT;

int canonicalize(double, double) __NOEXCEPT;

int canonicalizef(float, float) __NOEXCEPT;

int canonicalizel(long double, long double) __NOEXCEPT;

double cbrt(double) __NOEXCEPT;

float cbrtf(float) __NOEXCEPT;

double ceil(double) __NOEXCEPT;

float ceilf(float) __NOEXCEPT;

long double ceill(long double) __NOEXCEPT;

double copysign(double, double) __NOEXCEPT;

float copysignf(float, float) __NOEXCEPT;

long double copysignl(long double, long double) __NOEXCEPT;

double cos(double) __NOEXCEPT;

float cosf(float) __NOEXCEPT;

float coshf(float) __NOEXCEPT;

long double ddivl(long double, long double) __NOEXCEPT;

double dmull(long double, long double) __NOEXCEPT;

double dsqrtl(long double) __NOEXCEPT;

float erff(float) __NOEXCEPT;

double exp(double) __NOEXCEPT;

double exp10(double) __NOEXCEPT;

float exp10f(float) __NOEXCEPT;

float exp10m1f(float) __NOEXCEPT;

double exp2(double) __NOEXCEPT;

float exp2f(float) __NOEXCEPT;

float exp2m1f(float) __NOEXCEPT;

float expf(float) __NOEXCEPT;

double expm1(double) __NOEXCEPT;

float expm1f(float) __NOEXCEPT;

double fabs(double) __NOEXCEPT;

float fabsf(float) __NOEXCEPT;

long double fabsl(long double) __NOEXCEPT;

float fadd(double, double) __NOEXCEPT;

float faddl(long double, long double) __NOEXCEPT;

double fdim(double, double) __NOEXCEPT;

float fdimf(float, float) __NOEXCEPT;

long double fdiml(long double, long double) __NOEXCEPT;

float fdiv(double, double) __NOEXCEPT;

float fdivl(long double, long double) __NOEXCEPT;

float ffma(double, double, double) __NOEXCEPT;

float ffmal(long double, long double, long double) __NOEXCEPT;

double floor(double) __NOEXCEPT;

float floorf(float) __NOEXCEPT;

long double floorl(long double) __NOEXCEPT;

double fma(double, double, double) __NOEXCEPT;

float fmaf(float, float, float) __NOEXCEPT;

double fmax(double, double) __NOEXCEPT;

float fmaxf(float, float) __NOEXCEPT;

double fmaximum(double, double) __NOEXCEPT;

double fmaximum_mag(double, double) __NOEXCEPT;

double fmaximum_mag_num(double, double) __NOEXCEPT;

float fmaximum_mag_numf(float, float) __NOEXCEPT;

long double fmaximum_mag_numl(long double, long double) __NOEXCEPT;

float fmaximum_magf(float, float) __NOEXCEPT;

long double fmaximum_magl(long double, long double) __NOEXCEPT;

double fmaximum_num(double, double) __NOEXCEPT;

float fmaximum_numf(float, float) __NOEXCEPT;

long double fmaximum_numl(long double, long double) __NOEXCEPT;

float fmaximumf(float, float) __NOEXCEPT;

long double fmaximuml(long double, long double) __NOEXCEPT;

long double fmaxl(long double, long double) __NOEXCEPT;

double fmin(double, double) __NOEXCEPT;

float fminf(float, float) __NOEXCEPT;

double fminimum(double, double) __NOEXCEPT;

double fminimum_mag(double, double) __NOEXCEPT;

double fminimum_mag_num(double, double) __NOEXCEPT;

float fminimum_mag_numf(float, float) __NOEXCEPT;

long double fminimum_mag_numl(long double, long double) __NOEXCEPT;

float fminimum_magf(float, float) __NOEXCEPT;

long double fminimum_magl(long double, long double) __NOEXCEPT;

double fminimum_num(double, double) __NOEXCEPT;

float fminimum_numf(float, float) __NOEXCEPT;

float fminimumf(float, float) __NOEXCEPT;

long double fminimuml(long double, long double) __NOEXCEPT;

long double fminl(long double, long double) __NOEXCEPT;

double fmod(double, double) __NOEXCEPT;

float fmodf(float, float) __NOEXCEPT;

float fmul(double, double) __NOEXCEPT;

float fmull(long double, long double) __NOEXCEPT;

double frexp(double, int *) __NOEXCEPT;

float frexpf(float, int *) __NOEXCEPT;

long double frexpl(long double, int *) __NOEXCEPT;

double fromfp(double, int, unsigned int) __NOEXCEPT;

float fromfpf(float, int, unsigned int) __NOEXCEPT;

long double fromfpl(long double, int, unsigned int) __NOEXCEPT;

double fromfpx(double, int, unsigned int) __NOEXCEPT;

float fromfpxf(float, int, unsigned int) __NOEXCEPT;

long double fromfpxl(long double, int, unsigned int) __NOEXCEPT;

float fsqrt(double) __NOEXCEPT;

float fsqrtl(long double) __NOEXCEPT;

float fsub(double, double) __NOEXCEPT;

float fsubl(long double, long double) __NOEXCEPT;

double getpayload(double *) __NOEXCEPT;

float getpayloadf(float *) __NOEXCEPT;

long double getpayloadl(long double *) __NOEXCEPT;

double hypot(double, double) __NOEXCEPT;

float hypotf(float, float) __NOEXCEPT;

int ilogb(double) __NOEXCEPT;

int ilogbf(float) __NOEXCEPT;

int ilogbl(long double) __NOEXCEPT;

int isnan(double) __NOEXCEPT;

int isnanf(float) __NOEXCEPT;

int isnanl(long double) __NOEXCEPT;

double ldexp(double, int) __NOEXCEPT;

float ldexpf(float, int) __NOEXCEPT;

long double ldexpl(long double, int) __NOEXCEPT;

long llogb(double) __NOEXCEPT;

long llogbf(float) __NOEXCEPT;

long llogbl(long double) __NOEXCEPT;

long long llrint(double) __NOEXCEPT;

long long llrintf(float) __NOEXCEPT;

long long llrintl(long double) __NOEXCEPT;

long long llround(double) __NOEXCEPT;

long long llroundf(float) __NOEXCEPT;

long long llroundl(long double) __NOEXCEPT;

double log(double) __NOEXCEPT;

double log10(double) __NOEXCEPT;

float log10f(float) __NOEXCEPT;

double log1p(double) __NOEXCEPT;

float log1pf(float) __NOEXCEPT;

double log2(double) __NOEXCEPT;

float log2f(float) __NOEXCEPT;

double logb(double) __NOEXCEPT;

float logbf(float) __NOEXCEPT;

long double logbl(long double) __NOEXCEPT;

float logf(float) __NOEXCEPT;

long lrint(double) __NOEXCEPT;

long lrintf(float) __NOEXCEPT;

long lrintl(long double) __NOEXCEPT;

long lround(double) __NOEXCEPT;

long lroundf(float) __NOEXCEPT;

long lroundl(long double) __NOEXCEPT;

double modf(double, double *) __NOEXCEPT;

float modff(float, float *) __NOEXCEPT;

long double modfl(long double, long double *) __NOEXCEPT;

double nan(const char *) __NOEXCEPT;

float nanf(const char *) __NOEXCEPT;

long double nanl(const char *) __NOEXCEPT;

double nearbyint(double) __NOEXCEPT;

float nearbyintf(float) __NOEXCEPT;

long double nearbyintl(long double) __NOEXCEPT;

double nextafter(double, double) __NOEXCEPT;

float nextafterf(float, float) __NOEXCEPT;

long double nextafterl(long double, long double) __NOEXCEPT;

double nextdown(double) __NOEXCEPT;

float nextdownf(float) __NOEXCEPT;

long double nextdownl(long double) __NOEXCEPT;

double nexttoward(double, long double) __NOEXCEPT;

float nexttowardf(float, long double) __NOEXCEPT;

long double nexttowardl(long double, long double) __NOEXCEPT;

double nextup(double) __NOEXCEPT;

float nextupf(float) __NOEXCEPT;

long double nextupl(long double) __NOEXCEPT;

double pow(double, double) __NOEXCEPT;

float powf(float, float) __NOEXCEPT;

double remainder(double, double) __NOEXCEPT;

float remainderf(float, float) __NOEXCEPT;

long double remainderl(long double, long double) __NOEXCEPT;

double remquo(double, double, int *) __NOEXCEPT;

float remquof(float, float, int *) __NOEXCEPT;

long double remquol(long double, long double, int *) __NOEXCEPT;

double rint(double) __NOEXCEPT;

float rintf(float) __NOEXCEPT;

long double rintl(long double) __NOEXCEPT;

double round(double) __NOEXCEPT;

double roundeven(double) __NOEXCEPT;

float roundevenf(float) __NOEXCEPT;

long double roundevenl(long double) __NOEXCEPT;

float roundf(float) __NOEXCEPT;

long double roundl(long double) __NOEXCEPT;

double scalbln(double, long) __NOEXCEPT;

float scalblnf(float, long) __NOEXCEPT;

long double scalblnl(long double, long) __NOEXCEPT;

double scalbn(double, int) __NOEXCEPT;

float scalbnf(float, int) __NOEXCEPT;

long double scalbnl(long double, int) __NOEXCEPT;

int setpayload(double *, double) __NOEXCEPT;

int setpayloadf(float *, float) __NOEXCEPT;

int setpayloadl(long double *, long double) __NOEXCEPT;

double setpayloadsig(double *, double) __NOEXCEPT;

int setpayloadsigf(float *, float) __NOEXCEPT;

int setpayloadsigl(long double *, long double) __NOEXCEPT;

double sin(double) __NOEXCEPT;

void sincosf(float, float *, float *) __NOEXCEPT;

float sinf(float) __NOEXCEPT;

float sinhf(float) __NOEXCEPT;

double sqrt(double) __NOEXCEPT;

float sqrtf(float) __NOEXCEPT;

long double sqrtl(long double) __NOEXCEPT;

double tan(double) __NOEXCEPT;

float tanf(float) __NOEXCEPT;

float tanhf(float) __NOEXCEPT;

int totalorder(const double *, const double *) __NOEXCEPT;

int totalorderf(const float *, const float *) __NOEXCEPT;

int totalorderl(const long double *, const long double *) __NOEXCEPT;

int totalordermag(const double *, const double *) __NOEXCEPT;

int totalordermagf(const float *, const float *) __NOEXCEPT;

int totalordermagl(const long double *, const long double *) __NOEXCEPT;

double trunc(double) __NOEXCEPT;

float truncf(float) __NOEXCEPT;

long double truncl(long double) __NOEXCEPT;

double ufromfp(double, int, unsigned int) __NOEXCEPT;

float ufromfpf(float, int, unsigned int) __NOEXCEPT;

long double ufromfpl(long double, int, unsigned int) __NOEXCEPT;

double ufromfpx(double, int, unsigned int) __NOEXCEPT;

float ufromfpxf(float, int, unsigned int) __NOEXCEPT;

long double ufromfpxl(long double, int, unsigned int) __NOEXCEPT;

__END_C_DECLS


#include "llvm-libc-macros/math-function-macros.h"

#endif // LLVM_LIBC_MATH_H
