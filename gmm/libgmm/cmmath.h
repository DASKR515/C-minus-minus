/*
              --    by JABIR aka <DASKR>
              --    This cmmath.h file is a wrapper for the C Math library for C-- .
              --    Names follow the 'm' prefix convention to keep your FFI boilerplate clean.
              --    ver <1.0.0>
*/

/*
#pragma once
#include "Cmm.h"

#define mcos(x)                (r) = foreign "C" cos(x)
#define msin(x)                (r) = foreign "C" sin(x)
#define mtan(x)                (r) = foreign "C" tan(x)
#define macos(x)               (r) = foreign "C" acos(x)
#define masin(x)               (r) = foreign "C" asin(x)
#define matan(x)               (r) = foreign "C" atan(x)
#define matan2(y, x)           (r) = foreign "C" atan2(y, x)

#define mcosh(x)               (r) = foreign "C" cosh(x)
#define msinh(x)               (r) = foreign "C" sinh(x)
#define mtanh(x)               (r) = foreign "C" tanh(x)
#define macosh(x)              (r) = foreign "C" acosh(x)
#define masinh(x)              (r) = foreign "C" asinh(x)
#define matanh(x)              (r) = foreign "C" atanh(x)

#define mexp(x)                (r) = foreign "C" exp(x)
#define mexp2(x)               (r) = foreign "C" exp2(x)
#define mexpm1(x)              (r) = foreign "C" expm1(x)
#define mlog(x)                (r) = foreign "C" log(x)
#define mlog10(x)              (r) = foreign "C" log10(x)
#define mlog2(x)               (r) = foreign "C" log2(x)
#define mlog1p(x)              (r) = foreign "C" log1p(x)
#define mlogb(x)               (r) = foreign "C" logb(x)

#define milogb(x)              (ilg) = foreign "C" ilogb(x)
#define mlround(x)             (li) = foreign "C" lround(x)
#define mllround(x)            (li) = foreign "C" llround(x)
#define mlrint(x)              (li) = foreign "C" lrint(x)
#define mllrint(x)             (li) = foreign "C" llrint(x)

#define mmodf(x, iptr)         (r) = foreign "C" modf(x, iptr)
#define mfrexp(x, exp)         (r) = foreign "C" frexp(x, exp)
#define mldexp(x, exp)         (r) = foreign "C" ldexp(x, exp)
#define mscalbn(x, exp)        (r) = foreign "C" scalbn(x, exp)
#define mscalbln(x, exp)       (r) = foreign "C" scalbln(x, exp)

#define msqrt(r, x)            (r) = foreign "C" sqrt(x)
#define mcbrt(r, x)            (r) = foreign "C" cbrt(x)
#define mpow(r, base, exp)     (r) = foreign "C" pow(base, exp)

#define mhypot(x, y)           (r) = foreign "C" hypot(x, y)
#define mfabs(x)               (r) = foreign "C" fabs(x)

#define mceil(x)               (r) = foreign "C" ceil(x)
#define mfloor(x)              (r) = foreign "C" floor(x)
#define mtrunc(x)              (r) = foreign "C" trunc(x)
#define mround(x)              (r) = foreign "C" round(x)
#define mrint(x)               (r) = foreign "C" rint(x)
#define mnearbyint(x)          (r) = foreign "C" nearbyint(x)

#define mfmod(numer, denom)    (r) = foreign "C" fmod(numer, denom)
#define mremainder(x, y)       (r) = foreign "C" remainder(x, y)
#define mremquo(x, y, quo)     (r) = foreign "C" remquo(x, y, quo)

#define mcopysign(x, y)        (r) = foreign "C" copysign(x, y)
#define mnan(tagp)             (r) = foreign "C" nan(tagp)
#define mnextafter(x, y)       (r) = foreign "C" nextafter(x, y)
#define mnexttoward(x, y)      (r) = foreign "C" nexttoward(x, y)

#define mfdim(x, y)            (r) = foreign "C" fdim(x, y)
#define mfmax(x, y)            (r) = foreign "C" fmax(x, y)
#define mfmin(x, y)            (r) = foreign "C" fmin(x, y)

#define mfpclassify(x)         foreign "C" fpclassify(x)
#define misfinite(x)           foreign "C" isfinite(x)
#define misinf(x)              foreign "C" isinf(x)
#define misnan(x)              foreign "C" isnan(x)
#define mnormal(x)             foreign "C" isnormal(x)
#define msignbit(x)            foreign "C" signbit(x)

#define merf(x)                (r) = foreign "C" erf(x)
#define merfc(x)               (r) = foreign "C" erfc(x)
#define mtgamma(x)             (r) = foreign "C" tgamma(x)
#define mlgamma(x)             (r) = foreign "C" lgamma(x)


#define mPI_HEX      0x400921fb54442d18
#define mE_HEX       0x4005bf0a8b145769
#define mSQRT2_HEX   0x403f6a09e667f3bcd
*/
