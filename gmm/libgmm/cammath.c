/*
This module avoids all direct float64 arithmetic in Cmm due to a confirmed
GHC Native Code Generator code generation bug.

The NCG incorrectly emits addq for XMM registers when floating-point
operations are written directly in Cmm.

Therefore every floating-point operation is executed entirely inside C.

Cmm only passes memory addresses.
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

void testx(void* addr, const char* decstr) {
    double d = strtod(decstr, NULL);
    memcpy(addr, &d, sizeof(d));
}

void madd(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = da + db;
    memcpy(result, &r, sizeof(r));
}

void msub(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = da - db;
    memcpy(result, &r, sizeof(r));
}

void mmul(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = da * db;
    memcpy(result, &r, sizeof(r));
}

void mdiv(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = da / db;
    memcpy(result, &r, sizeof(r));
}

void msin(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = sin(da);
    memcpy(result, &r, sizeof(r));
}

void mcos(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = cos(da);
    memcpy(result, &r, sizeof(r));
}

void mtan(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = tan(da);
    memcpy(result, &r, sizeof(r));
}

void masin(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = asin(da);
    memcpy(result, &r, sizeof(r));
}

void macos(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = acos(da);
    memcpy(result, &r, sizeof(r));
}

void matan(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = atan(da);
    memcpy(result, &r, sizeof(r));
}

void matan2(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = atan2(da, db);
    memcpy(result, &r, sizeof(r));
}

void msinh(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = sinh(da);
    memcpy(result, &r, sizeof(r));
}

void mcosh(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = cosh(da);
    memcpy(result, &r, sizeof(r));
}

void mtanh(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = tanh(da);
    memcpy(result, &r, sizeof(r));
}

void masinh(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = asinh(da);
    memcpy(result, &r, sizeof(r));
}

void macosh(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = acosh(da);
    memcpy(result, &r, sizeof(r));
}

void matanh(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = atanh(da);
    memcpy(result, &r, sizeof(r));
}

void mexp(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = exp(da);
    memcpy(result, &r, sizeof(r));
}

void mexp2(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = exp2(da);
    memcpy(result, &r, sizeof(r));
}

void mexpm1(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = expm1(da);
    memcpy(result, &r, sizeof(r));
}

void mlog(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = log(da);
    memcpy(result, &r, sizeof(r));
}

void mlog10(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = log10(da);
    memcpy(result, &r, sizeof(r));
}

void mlog2(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = log2(da);
    memcpy(result, &r, sizeof(r));
}

void mlog1p(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = log1p(da);
    memcpy(result, &r, sizeof(r));
}

void mlogb(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = logb(da);
    memcpy(result, &r, sizeof(r));
}

void milogb(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    int r = ilogb(da);
    memcpy(result, &r, sizeof(r));
}

void mpow(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = pow(da, db);
    memcpy(result, &r, sizeof(r));
}

void msqrt(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = sqrt(da);
    memcpy(result, &r, sizeof(r));
}

void mcbrt(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = cbrt(da);
    memcpy(result, &r, sizeof(r));
}

void mhypot(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = hypot(da, db);
    memcpy(result, &r, sizeof(r));
}

void mtgamma(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = tgamma(da);
    memcpy(result, &r, sizeof(r));
}

void mlgamma(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = lgamma(da);
    memcpy(result, &r, sizeof(r));
}

void merf(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = erf(da);
    memcpy(result, &r, sizeof(r));
}

void merfc(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = erfc(da);
    memcpy(result, &r, sizeof(r));
}

void mfabs(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = fabs(da);
    memcpy(result, &r, sizeof(r));
}

void mcopysign(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = copysign(da, db);
    memcpy(result, &r, sizeof(r));
}

void mfmod(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = fmod(da, db);
    memcpy(result, &r, sizeof(r));
}

void mremainder(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = remainder(da, db);
    memcpy(result, &r, sizeof(r));
}

void mremquo(void* result, void* quo, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    int q;
    double r = remquo(da, db, &q);
    memcpy(result, &r, sizeof(r));
    memcpy(quo, &q, sizeof(q));
}

void mfmax(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = fmax(da, db);
    memcpy(result, &r, sizeof(r));
}

void mfmin(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = fmin(da, db);
    memcpy(result, &r, sizeof(r));
}

void mfdim(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = fdim(da, db);
    memcpy(result, &r, sizeof(r));
}

void mfrexp(void* result, void* exponent, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    int e;
    double r = frexp(da, &e);
    memcpy(result, &r, sizeof(r));
    memcpy(exponent, &e, sizeof(e));
}

void mmodf(void* frac, void* integer, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double i;
    double r = modf(da, &i);
    memcpy(frac, &r, sizeof(r));
    memcpy(integer, &i, sizeof(i));
}

void mldexp(void* result, void* a, int exp) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = ldexp(da, exp);
    memcpy(result, &r, sizeof(r));
}

void mscalbn(void* result, void* a, int exp) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = scalbn(da, exp);
    memcpy(result, &r, sizeof(r));
}

void mscalbln(void* result, void* a, long exp) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = scalbln(da, exp);
    memcpy(result, &r, sizeof(r));
}

void mnextafter(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = nextafter(da, db);
    memcpy(result, &r, sizeof(r));
}

void mnexttoward(void* result, void* a, void* b) {
    double da;
    long double db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    double r = nexttoward(da, db);
    memcpy(result, &r, sizeof(r));
}

void mfma(void* result, void* a, void* b, void* c) {
    double da, db, dc;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    memcpy(&dc, c, sizeof(dc));
    double r = fma(da, db, dc);
    memcpy(result, &r, sizeof(r));
}

void mnan(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = nan((const char*)&da);
    memcpy(result, &r, sizeof(r));
}

void mceil(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = ceil(da);
    memcpy(result, &r, sizeof(r));
}

void mfloor(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = floor(da);
    memcpy(result, &r, sizeof(r));
}

void mtrunc(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = trunc(da);
    memcpy(result, &r, sizeof(r));
}

void mround(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = round(da);
    memcpy(result, &r, sizeof(r));
}

void mlround(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    long r = lround(da);
    memcpy(result, &r, sizeof(r));
}

void mllround(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    long long r = llround(da);
    memcpy(result, &r, sizeof(r));
}

void mrint(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = rint(da);
    memcpy(result, &r, sizeof(r));
}

void mlrint(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    long r = lrint(da);
    memcpy(result, &r, sizeof(r));
}

void mllrint(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    long long r = llrint(da);
    memcpy(result, &r, sizeof(r));
}

void mnearbyint(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    double r = nearbyint(da);
    memcpy(result, &r, sizeof(r));
}

void mfpclassify(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    int r = fpclassify(da);
    memcpy(result, &r, sizeof(r));
}

void misfinite(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    int r = isfinite(da);
    memcpy(result, &r, sizeof(r));
}

void misinf(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    int r = isinf(da);
    memcpy(result, &r, sizeof(r));
}

void misnan(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    int r = isnan(da);
    memcpy(result, &r, sizeof(r));
}

void misnormal(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    int r = isnormal(da);
    memcpy(result, &r, sizeof(r));
}

void msignbit(void* result, void* a) {
    double da;
    memcpy(&da, a, sizeof(da));
    int r = signbit(da);
    memcpy(result, &r, sizeof(r));
}

void misgreater(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    int r = isgreater(da, db);
    memcpy(result, &r, sizeof(r));
}

void misgreaterequal(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    int r = isgreaterequal(da, db);
    memcpy(result, &r, sizeof(r));
}

void misless(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    int r = isless(da, db);
    memcpy(result, &r, sizeof(r));
}

void mislessequal(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    int r = islessequal(da, db);
    memcpy(result, &r, sizeof(r));
}

void mislessgreater(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    int r = islessgreater(da, db);
    memcpy(result, &r, sizeof(r));
}

void misunordered(void* result, void* a, void* b) {
    double da, db;
    memcpy(&da, a, sizeof(da));
    memcpy(&db, b, sizeof(db));
    int r = isunordered(da, db);
    memcpy(result, &r, sizeof(r));
}
