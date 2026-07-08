cmmath.h API Documentation

Introduction

cmmath.h provides a comprehensive set of mathematical functions for C-- through a simple, consistent API. All functions operate on double-precision floating-point values and follow the m prefix convention to maintain clean FFI boilerplate. The API mirrors standard C mathematical operations while avoiding direct float64 arithmetic in Cmm due to known compiler limitations.

---

Basic Arithmetic

madd

Description

Performs addition of two floating-point values. This function computes the sum of two numbers and stores the result in the destination variable.

Parameters

Parameter Description
result Destination where the sum will be stored.
a First operand to be added.
b Second operand to be added.

Result

Produces the sum of a and b. The result is a double-precision floating-point value.

Notes

This operation follows standard floating-point addition rules, including handling of special values such as infinities and NaN.

---

msub

Description

Performs subtraction of two floating-point values. This function computes the difference between two numbers and stores the result in the destination variable.

Parameters

Parameter Description
result Destination where the difference will be stored.
a Operand from which the subtraction is performed (minuend).
b Operand to be subtracted (subtrahend).

Result

Produces the result of a - b. The result is a double-precision floating-point value.

---

mmul

Description

Performs multiplication of two floating-point values. This function computes the product of two numbers and stores the result in the destination variable.

Parameters

Parameter Description
result Destination where the product will be stored.
a First operand to be multiplied.
b Second operand to be multiplied.

Result

Produces the product of a and b. The result is a double-precision floating-point value.

---

mfdiv

Description

Performs division of two floating-point values. This function computes the quotient of two numbers and stores the result in the destination variable.

Parameters

Parameter Description
result Destination where the quotient will be stored.
a Numerator (dividend).
b Denominator (divisor).

Result

Produces the result of a / b. The result is a double-precision floating-point value.

Notes

Division by zero produces a signed infinity. If both operands are zero, the result is NaN.

---

Trigonometric Functions

msin

Description

Computes the sine of a given angle. The function calculates the trigonometric sine of the specified value.

Parameters

Parameter Description
result Destination where the sine value will be stored.
a Angle in radians.

Result

Returns the sine of a. The result is a double-precision floating-point value in the range [-1, 1].

---

mcos

Description

Computes the cosine of a given angle. The function calculates the trigonometric cosine of the specified value.

Parameters

Parameter Description
result Destination where the cosine value will be stored.
a Angle in radians.

Result

Returns the cosine of a. The result is a double-precision floating-point value in the range [-1, 1].

---

mtan

Description

Computes the tangent of a given angle. The function calculates the trigonometric tangent of the specified value.

Parameters

Parameter Description
result Destination where the tangent value will be stored.
a Angle in radians.

Result

Returns the tangent of a. The result is a double-precision floating-point value.

Notes

The tangent function has vertical asymptotes at π/2 + kπ where k is an integer. Values near these points may produce very large results or infinities.

---

masin

Description

Computes the inverse sine (arcsine) of a given value. This function returns the angle whose sine equals the specified value.

Parameters

Parameter Description
result Destination where the arcsine value will be stored.
a Value for which the inverse sine is calculated.

Result

Returns the arcsine of a in radians. The result is in the range [-π/2, π/2].

Notes

The input must be in the range [-1, 1]. Values outside this domain produce NaN.

---

macos

Description

Computes the inverse cosine (arccosine) of a given value. This function returns the angle whose cosine equals the specified value.

Parameters

Parameter Description
result Destination where the arccosine value will be stored.
a Value for which the inverse cosine is calculated.

Result

Returns the arccosine of a in radians. The result is in the range [0, π].

Notes

The input must be in the range [-1, 1]. Values outside this domain produce NaN.

---

matan

Description

Computes the inverse tangent (arctangent) of a given value. This function returns the angle whose tangent equals the specified value.

Parameters

Parameter Description
result Destination where the arctangent value will be stored.
a Value for which the inverse tangent is calculated.

Result

Returns the arctangent of a in radians. The result is in the range [-π/2, π/2].

---

matan2

Description

Computes the inverse tangent of the quotient of two values. This function calculates the angle whose tangent equals a / b, using the signs of both arguments to determine the correct quadrant.

Parameters

Parameter Description
result Destination where the arctangent value will be stored.
a y-coordinate or numerator.
b x-coordinate or denominator.

Result

Returns the arctangent of a / b in radians. The result is in the range [-π, π].

Notes

This function correctly handles all four quadrants. If both arguments are zero, the result is undefined (typically 0 or NaN).

---

Hyperbolic Functions

msinh

Description

Computes the hyperbolic sine of a given value. This function calculates sinh(a) = (e^a - e^(-a))/2.

Parameters

Parameter Description
result Destination where the hyperbolic sine will be stored.
a Input value.

Result

Returns the hyperbolic sine of a. The result is a double-precision floating-point value.

---

mcosh

Description

Computes the hyperbolic cosine of a given value. This function calculates cosh(a) = (e^a + e^(-a))/2.

Parameters

Parameter Description
result Destination where the hyperbolic cosine will be stored.
a Input value.

Result

Returns the hyperbolic cosine of a. The result is always greater than or equal to 1.

---

mtanh

Description

Computes the hyperbolic tangent of a given value. This function calculates tanh(a) = sinh(a) / cosh(a).

Parameters

Parameter Description
result Destination where the hyperbolic tangent will be stored.
a Input value.

Result

Returns the hyperbolic tangent of a. The result is in the range [-1, 1].

---

masinh

Description

Computes the inverse hyperbolic sine of a given value. This function calculates asinh(a) = log(a + sqrt(a^2 + 1)).

Parameters

Parameter Description
result Destination where the inverse hyperbolic sine will be stored.
a Input value.

Result

Returns the inverse hyperbolic sine of a. The result is a double-precision floating-point value.

Notes

This function is defined for all real numbers.

---

macosh

Description

Computes the inverse hyperbolic cosine of a given value. This function calculates acosh(a) = log(a + sqrt(a^2 - 1)).

Parameters

Parameter Description
result Destination where the inverse hyperbolic cosine will be stored.
a Input value.

Result

Returns the inverse hyperbolic cosine of a. The result is a double-precision floating-point value.

Notes

The input must be greater than or equal to 1. Values less than 1 produce NaN.

---

matanh

Description

Computes the inverse hyperbolic tangent of a given value. This function calculates atanh(a) = 0.5 * log((1 + a)/(1 - a)).

Parameters

Parameter Description
result Destination where the inverse hyperbolic tangent will be stored.
a Input value.

Result

Returns the inverse hyperbolic tangent of a. The result is a double-precision floating-point value.

Notes

The input must be in the range (-1, 1). Values outside this open interval produce NaN or infinity.

---

Exponential Functions

mexp

Description

Computes the base-e exponential of a given value. This function calculates e^a.

Parameters

Parameter Description
result Destination where the exponential value will be stored.
a Input exponent.

Result

Returns e^a. The result is a double-precision floating-point value.

Notes

For very large values, the result may overflow to infinity.

---

mexp2

Description

Computes the base-2 exponential of a given value. This function calculates 2^a.

Parameters

Parameter Description
result Destination where the exponential value will be stored.
a Input exponent.

Result

Returns 2^a. The result is a double-precision floating-point value.

---

mexpm1

Description

Computes the base-e exponential of a given value minus one. This function calculates e^a - 1 accurately, even for very small values of a.

Parameters

Parameter Description
result Destination where the result will be stored.
a Input exponent.

Result

Returns e^a - 1. The result is a double-precision floating-point value.

Notes

This function is designed to provide accurate results when a is close to zero, avoiding cancellation errors.

---

Logarithmic Functions

mlog

Description

Computes the natural logarithm (base-e) of a given value. This function calculates log(a).

Parameters

Parameter Description
result Destination where the logarithm will be stored.
a Input value.

Result

Returns the natural logarithm of a. The result is a double-precision floating-point value.

Notes

The input must be positive. Values less than or equal to zero produce NaN or negative infinity.

---

mlog10

Description

Computes the base-10 logarithm of a given value. This function calculates log10(a).

Parameters

Parameter Description
result Destination where the logarithm will be stored.
a Input value.

Result

Returns the base-10 logarithm of a. The result is a double-precision floating-point value.

Notes

The input must be positive. Values less than or equal to zero produce NaN or negative infinity.

---

mlog2

Description

Computes the base-2 logarithm of a given value. This function calculates log2(a).

Parameters

Parameter Description
result Destination where the logarithm will be stored.
a Input value.

Result

Returns the base-2 logarithm of a. The result is a double-precision floating-point value.

Notes

The input must be positive. Values less than or equal to zero produce NaN or negative infinity.

---

mlog1p

Description

Computes the natural logarithm of one plus a given value. This function calculates log(1 + a) accurately, even for very small values of a.

Parameters

Parameter Description
result Destination where the logarithm will be stored.
a Input value.

Result

Returns log(1 + a). The result is a double-precision floating-point value.

Notes

The input must be greater than -1. Values less than or equal to -1 produce NaN or negative infinity.

---

mlogb

Description

Computes the exponent of a given floating-point value. This function calculates the unbiased exponent of the input.

Parameters

Parameter Description
result Destination where the exponent will be stored.
a Input value.

Result

Returns the exponent of a as a floating-point value. The result is a double-precision value.

---

milogb

Description

Computes the exponent of a given floating-point value as an integer. This function returns the unbiased exponent of the input as an integer.

Parameters

Parameter Description
result Destination where the exponent will be stored.
a Input value.

Result

Returns the exponent of a as an integer. The result is stored in an integer format.

Notes

For special values such as zero or NaN, appropriate integer values are returned.

---

Power Functions

mpow

Description

Computes a value raised to a specified power. This function calculates a^b.

Parameters

Parameter Description
result Destination where the power will be stored.
a Base value.
b Exponent value.

Result

Returns a^b. The result is a double-precision floating-point value.

Notes

Domain restrictions apply depending on the base and exponent values. Negative bases with non-integer exponents are invalid and produce NaN.

---

msqrt

Description

Computes the square root of a given value. This function calculates sqrt(a).

Parameters

Parameter Description
result Destination where the square root will be stored.
a Input value.

Result

Returns the square root of a. The result is a double-precision floating-point value.

Notes

The input must be non-negative. Negative inputs produce NaN.

---

mcbrt

Description

Computes the cube root of a given value. This function calculates cbrt(a).

Parameters

Parameter Description
result Destination where the cube root will be stored.
a Input value.

Result

Returns the cube root of a. The result is a double-precision floating-point value.

Notes

This function works for both positive and negative values.

---

mhypot

Description

Computes the hypotenuse of a right triangle. This function calculates sqrt(a^2 + b^2) accurately, avoiding overflow and underflow.

Parameters

Parameter Description
result Destination where the hypotenuse will be stored.
a Length of one leg.
b Length of the other leg.

Result

Returns the length of the hypotenuse. The result is a double-precision floating-point value.

---

Gamma Functions

mtgamma

Description

Computes the gamma function of a given value. This function calculates Γ(a).

Parameters

Parameter Description
result Destination where the gamma value will be stored.
a Input value.

Result

Returns the gamma function of a. The result is a double-precision floating-point value.

Notes

The function has poles at non-positive integers. Values near these points may produce very large results or infinities.

---

mlgamma

Description

Computes the natural logarithm of the absolute value of the gamma function. This function calculates log(|Γ(a)|).

Parameters

Parameter Description
result Destination where the log-gamma value will be stored.
a Input value.

Result

Returns the natural logarithm of the absolute value of the gamma function. The result is a double-precision floating-point value.

---

Error Functions

merf

Description

Computes the error function of a given value. This function calculates erf(a) = (2/√π) * ∫[0 to a] e^(-t^2) dt.

Parameters

Parameter Description
result Destination where the error function value will be stored.
a Input value.

Result

Returns the error function of a. The result is in the range [-1, 1].

---

merfc

Description

Computes the complementary error function of a given value. This function calculates erfc(a) = 1 - erf(a).

Parameters

Parameter Description
result Destination where the complementary error function value will be stored.
a Input value.

Result

Returns the complementary error function of a. The result is in the range [0, 2].

---

Absolute Value and Sign Functions

mfabs

Description

Computes the absolute value of a floating-point number. This function returns the magnitude of the input, ignoring its sign.

Parameters

Parameter Description
result Destination where the absolute value will be stored.
a Input value.

Result

Returns the absolute value of a. The result is a non-negative double-precision floating-point value.

---

mcopysign

Description

Copies the sign of one floating-point number to another. This function returns a value with the magnitude of a and the sign of b.

Parameters

Parameter Description
result Destination where the result will be stored.
a Value whose magnitude will be used.
b Value whose sign will be used.

Result

Returns a value with the magnitude of a and the sign of b. The result is a double-precision floating-point value.

---

Remainder Functions

mfmod

Description

Computes the floating-point remainder of a division. This function calculates a - trunc(a/b) * b.

Parameters

Parameter Description
result Destination where the remainder will be stored.
a Dividend.
b Divisor.

Result

Returns the remainder of a/b. The result has the same sign as a.

---

mremainder

Description

Computes the remainder of a division according to IEEE 754. This function calculates a - round(a/b) * b.

Parameters

Parameter Description
result Destination where the remainder will be stored.
a Dividend.
b Divisor.

Result

Returns the IEEE remainder of a/b. The result is in the range [-b/2, b/2].

---

mremquo

Description

Computes the remainder and quotient of a division. This function calculates the remainder and stores the quotient as an integer.

Parameters

Parameter Description
result Destination where the remainder will be stored.
quo Destination where the quotient will be stored.
a Dividend.
b Divisor.

Result

Returns the remainder of a/b in the result parameter and stores the quotient in the quo parameter.

---

Maximum and Minimum Functions

mfmax

Description

Computes the larger of two floating-point values. This function returns the maximum of a and b.

Parameters

Parameter Description
result Destination where the maximum value will be stored.
a First value to compare.
b Second value to compare.

Result

Returns the larger of a and b. If one argument is NaN, the other argument is returned.

---

mfmin

Description

Computes the smaller of two floating-point values. This function returns the minimum of a and b.

Parameters

Parameter Description
result Destination where the minimum value will be stored.
a First value to compare.
b Second value to compare.

Result

Returns the smaller of a and b. If one argument is NaN, the other argument is returned.

---

mfdim

Description

Computes the positive difference between two floating-point values. This function returns max(a - b, 0).

Parameters

Parameter Description
result Destination where the positive difference will be stored.
a First value.
b Second value.

Result

Returns a - b if a > b, otherwise 0.

---

Floating-Point Decomposition

mfrexp

Description

Decomposes a floating-point value into its mantissa and exponent. This function separates the input into a normalized fraction and an integer exponent.

Parameters

Parameter Description
result Destination where the mantissa will be stored.
exp Destination where the exponent will be stored.
a Input value to decompose.

Result

The mantissa is returned in the range [0.5, 1.0) or [-1.0, -0.5], and the exponent is stored as an integer.

---

mmodf

Description

Decomposes a floating-point value into integer and fractional parts. This function separates the input into its integer and fractional components.

Parameters

Parameter Description
frac Destination where the fractional part will be stored.
integer Destination where the integer part will be stored.
a Input value to decompose.

Result

The fractional part is returned and the integer part is stored in the integer parameter.

---

mldexp

Description

Multiplies a floating-point value by a power of two. This function calculates a * 2^n.

Parameters

Parameter Description
result Destination where the result will be stored.
a Input value.
n Integer exponent.

Result

Returns a * 2^n. The result is a double-precision floating-point value.

---

mscalbn

Description

Multiplies a floating-point value by a power of two using an integer exponent. This function calculates a * 2^n.

Parameters

Parameter Description
result Destination where the result will be stored.
a Input value.
n Integer exponent.

Result

Returns a * 2^n. The result is a double-precision floating-point value.

---

mscalbln

Description

Multiplies a floating-point value by a power of two using a long integer exponent. This function calculates a * 2^n.

Parameters

Parameter Description
result Destination where the result will be stored.
a Input value.
n Long integer exponent.

Result

Returns a * 2^n. The result is a double-precision floating-point value.

---

Floating-Point Helper Functions

mnextafter

Description

Returns the next representable floating-point value after a given value in the direction of another. This function advances from a toward b to the next repre