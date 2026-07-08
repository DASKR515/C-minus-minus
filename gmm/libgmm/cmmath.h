/*
              --    by JABIR aka <DASKR>
              --    This cmmath.h file is a wrapper for the C Math library for C-- .
              --    Names follow the 'm' prefix convention to keep your FFI boilerplate clean.
              --    ver <1.0.4>
*/
#pragma once
#include "Cmm.h"


#undef mdiv


#define madd(result, ...) foreign "C" madd(result, __VA_ARGS__)
#define msub(result, ...) foreign "C" msub(result, __VA_ARGS__)
#define mmul(result, ...) foreign "C" mmul(result, __VA_ARGS__)
#define mfdiv(result, ...) foreign "C" mfdiv(result, __VA_ARGS__)

#define mE(result) foreign "C" mE(result)
#define mLOG2E(result) foreign "C" mLOG2E(result)
#define mLOG10E(result) foreign "C" mLOG10E(result)
#define mLN2(result) foreign "C" mLN2(result)
#define mLN10(result) foreign "C" mLN10(result)
#define mPI(result) foreign "C" mPI(result)
#define mPI_2(result) foreign "C" mPI_2(result)
#define mPI_4(result) foreign "C" mPI_4(result)
#define m1_PI(result) foreign "C" m1_PI(result)
#define m2_PI(result) foreign "C" m2_PI(result)
#define m2_SQRTPI(result) foreign "C" m2_SQRTPI(result)
#define mSQRT2(result) foreign "C" mSQRT2(result)
#define mSQRT1_2(result) foreign "C" mSQRT1_2(result)


#define msin(result,a) foreign "C" msin(result,a)
#define mcos(result,a) foreign "C" mcos(result,a)
#define mtan(result,a) foreign "C" mtan(result,a)
#define masin(result,a) foreign "C" masin(result,a)
#define macos(result,a) foreign "C" macos(result,a)
#define matan(result,a) foreign "C" matan(result,a)
#define matan2(result,a,b) foreign "C" matan2(result,a,b)

#define msinh(result,a) foreign "C" msinh(result,a)
#define mcosh(result,a) foreign "C" mcosh(result,a)
#define mtanh(result,a) foreign "C" mtanh(result,a)
#define masinh(result,a) foreign "C" masinh(result,a)
#define macosh(result,a) foreign "C" macosh(result,a)
#define matanh(result,a) foreign "C" matanh(result,a)


#define mexp(result,a) foreign "C" mexp(result,a)
#define mexp2(result,a) foreign "C" mexp2(result,a)
#define mexpm1(result,a) foreign "C" mexpm1(result,a)


#define mlog(result,a) foreign "C" mlog(result,a)
#define mlog10(result,a) foreign "C" mlog10(result,a)
#define mlog2(result,a) foreign "C" mlog2(result,a)
#define mlog1p(result,a) foreign "C" mlog1p(result,a)
#define mlogb(result,a) foreign "C" mlogb(result,a)
#define milogb(result,a) foreign "C" milogb(result,a)


#define mpow(result,a,b) foreign "C" mpow(result,a,b)
#define msqrt(result,a) foreign "C" msqrt(result,a)
#define mcbrt(result,a) foreign "C" mcbrt(result,a)
#define mhypot(result,a,b) foreign "C" mhypot(result,a,b)

#define mtgamma(result,a) foreign "C" mtgamma(result,a)
#define mlgamma(result,a) foreign "C" mlgamma(result,a)


#define merf(result,a) foreign "C" merf(result,a)
#define merfc(result,a) foreign "C" merfc(result,a)

#define mfabs(result,a) foreign "C" mfabs(result,a)
#define mcopysign(result,a,b) foreign "C" mcopysign(result,a,b)


#define mfmod(result,a,b) foreign "C" mfmod(result,a,b)
#define mremainder(result,a,b) foreign "C" mremainder(result,a,b)
#define mremquo(result,quo,a,b) foreign "C" mremquo(result,quo,a,b)


#define mfmax(result,a,b) foreign "C" mfmax(result,a,b)
#define mfmin(result,a,b) foreign "C" mfmin(result,a,b)
#define mfdim(result,a,b) foreign "C" mfdim(result,a,b)

#define mfrexp(result,exp,a) foreign "C" mfrexp(result,exp,a)
#define mmodf(frac,integer,a) foreign "C" mmodf(frac,integer,a)
#define mldexp(result,a,n) foreign "C" mldexp(result,a,n)
#define mscalbn(result,a,n) foreign "C" mscalbn(result,a,n)
#define mscalbln(result,a,n) foreign "C" mscalbln(result,a,n)


#define mnextafter(result,a,b) foreign "C" mnextafter(result,a,b)
#define mnexttoward(result,a,b) foreign "C" mnexttoward(result,a,b)
#define mfma(result,a,b,c) foreign "C" mfma(result,a,b,c)
#define mnan(result,a) foreign "C" mnan(result,a)


#define mceil(result,a) foreign "C" mceil(result,a)
#define mfloor(result,a) foreign "C" mfloor(result,a)
#define mtrunc(result,a) foreign "C" mtrunc(result,a)
#define mround(result,a) foreign "C" mround(result,a)
#define mlround(result,a) foreign "C" mlround(result,a)
#define mllround(result,a) foreign "C" mllround(result,a)
#define mrint(result,a) foreign "C" mrint(result,a)
#define mlrint(result,a) foreign "C" mlrint(result,a)
#define mllrint(result,a) foreign "C" mllrint(result,a)
#define mnearbyint(result,a) foreign "C" mnearbyint(result,a)

#define mfpclassify(result,a) foreign "C" mfpclassify(result,a)
#define misfinite(result,a) foreign "C" misfinite(result,a)
#define misinf(result,a) foreign "C" misinf(result,a)
#define misnan(result,a) foreign "C" misnan(result,a)
#define misnormal(result,a) foreign "C" misnormal(result,a)
#define msignbit(result,a) foreign "C" msignbit(result,a)
#define misgreater(result,a,b) foreign "C" misgreater(result,a,b)
#define misgreaterequal(result,a,b) foreign "C" misgreaterequal(result,a,b)
#define misless(result,a,b) foreign "C" misless(result,a,b)
#define mislessequal(result,a,b) foreign "C" mislessequal(result,a,b)
#define mislessgreater(result,a,b) foreign "C" mislessgreater(result,a,b)
#define misunordered(result,a,b) foreign "C" misunordered(result,a,b)
