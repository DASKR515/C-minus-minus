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
#include <stdarg.h>

/* Mathematical Constants - stored once as immutable values */
static const double CMM_E         = 2.7182818284590452353602874713526624977572470937000;
static const double CMM_LOG2E     = 1.4426950408889634073599246810018921374266459541530;
static const double CMM_LOG10E    = 0.43429448190325182765112891891660508229439700580367;
static const double CMM_LN2       = 0.69314718055994530941723212145817656807550013436026;
static const double CMM_LN10      = 2.3025850929940456840179914546843642076011014886288;
static const double CMM_PI        = 3.1415926535897932384626433832795028841971693993751;
static const double CMM_PI_2      = 1.5707963267948966192313216916397514420985846996876;
static const double CMM_PI_4      = 0.78539816339744830961566084581987572104929234984378;
static const double CMM_1_PI      = 0.31830988618379067153776752674502872406891929148091;
static const double CMM_2_PI      = 0.63661977236758134307553505349005744913783858296183;
static const double CMM_2_SQRTPI  = 1.1283791670955125738961589031215451716881012586580;
static const double CMM_SQRT2     = 1.4142135623730950488016887242096980785696718753769;
static const double CMM_SQRT1_2   = 0.70710678118654752440084436210484903928483593768847;

void testx(void* addr, const char* decstr) {
    double d = strtod(decstr, NULL);
    memcpy(addr, &d, sizeof(d));
}


static double process_va_list(double initial, va_list args, int count, char op) {
    double result = initial;
    int i;
    for (i = 0; i < count; i++) {
        void* ptr = va_arg(args, void*);
        double val;
        memcpy(&val, ptr, sizeof(val));
        switch(op) {
            case '+': result += val; break;
            case '-': result -= val; break;
            case '*': result *= val; break;
            case '/': result /= val; break;
        }
    }
    return result;
}

void madd(void* result, ...) {
    va_list args;
    va_start(args, result);
    
    void* a = va_arg(args, void*);
    double da;
    memcpy(&da, a, sizeof(da));
    
    int count = 0;
    va_list args_copy;
    va_copy(args_copy, args);
    while (1) {
        void* ptr = va_arg(args_copy, void*);
        if (ptr == NULL) break;
        count++;
    }
    va_end(args_copy);
    
    double r = process_va_list(da, args, count, '+');
    memcpy(result, &r, sizeof(r));
    va_end(args);
}

void msub(void* result, ...) {
    va_list args;
    va_start(args, result);
    
    void* a = va_arg(args, void*);
    double da;
    memcpy(&da, a, sizeof(da));
    
    int count = 0;
    va_list args_copy;
    va_copy(args_copy, args);
    while (1) {
        void* ptr = va_arg(args_copy, void*);
        if (ptr == NULL) break;
        count++;
    }
    va_end(args_copy);
    
    double r = process_va_list(da, args, count, '-');
    memcpy(result, &r, sizeof(r));
    va_end(args);
}

void mmul(void* result, ...) {
    va_list args;
    va_start(args, result);
    
    void* a = va_arg(args, void*);
    double da;
    memcpy(&da, a, sizeof(da));
    
    int count = 0;
    va_list args_copy;
    va_copy(args_copy, args);
    while (1) {
        void* ptr = va_arg(args_copy, void*);
        if (ptr == NULL) break;
        count++;
    }
    va_end(args_copy);
    
    double r = process_va_list(da, args, count, '*');
    memcpy(result, &r, sizeof(r));
    va_end(args);
}

void mfdiv(void* result, ...) {
    va_list args;
    va_start(args, result);
    
    void* a = va_arg(args, void*);
    double da;
    memcpy(&da, a, sizeof(da));
    
    int count = 0;
    va_list args_copy;
    va_copy(args_copy, args);
    while (1) {
        void* ptr = va_arg(args_copy, void*);
        if (ptr == NULL) break;
        count++;
    }
    va_end(args_copy);
    
    double r = process_va_list(da, args, count, '/');
    memcpy(result, &r, sizeof(r));
    va_end(args);
}

void mE(void* result) {
    memcpy(result, &CMM_E, sizeof(CMM_E));
}

void mLOG2E(void* result) {
    memcpy(result, &CMM_LOG2E, sizeof(CMM_LOG2E));
}

void mLOG10E(void* result) {
    memcpy(result, &CMM_LOG10E, sizeof(CMM_LOG10E));
}

void mLN2(void* result) {
    memcpy(result, &CMM_LN2, sizeof(CMM_LN2));
}

void mLN10(void* result) {
    memcpy(result, &CMM_LN10, sizeof(CMM_LN10));
}

void mPI(void* result) {
    memcpy(result, &CMM_PI, sizeof(CMM_PI));
}

void mPI_2(void* result) {
    memcpy(result, &CMM_PI_2, sizeof(CMM_PI_2));
}

void mPI_4(void* result) {
    memcpy(result, &CMM_PI_4, sizeof(CMM_PI_4));
}

void m1_PI(void* result) {
    memcpy(result, &CMM_1_PI, sizeof(CMM_1_PI));
}

void m2_PI(void* result) {
    memcpy(result, &CMM_2_PI, sizeof(CMM_2_PI));
}

void m2_SQRTPI(void* result) {
    memcpy(result, &CMM_2_SQRTPI, sizeof(CMM_2_SQRTPI));
}

void mSQRT2(void* result) {
    memcpy(result, &CMM_SQRT2, sizeof(CMM_SQRT2));
}

void mSQRT1_2(void* result) {
    memcpy(result, &CMM_SQRT1_2, sizeof(CMM_SQRT1_2));
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
