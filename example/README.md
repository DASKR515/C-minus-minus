# Cmm Complete Guide — From Basics to Every Construction in the Language

A complete educational reference for the **Cmm** language (GHC's C--), designed for any programmer who wants to learn the language from scratch — whether they have never dealt with it before or want a quick reference for every syntax construction present in it. Every example is an independent executable file built directly on the official GHC compiler grammar rules (`GHC/Cmm/Parser.y`) and tested practically.

All examples use the **`stdc--.h`** library to eliminate repetition (printing, strings, files...) instead of repeating `foreign "C"` for every small call

## Operating Requirements

Each example requires:
- `ghc` installed on the system.
- The `gmm` tool (or the manual alternative command below).

```bash
gmm -o app 01_hello_print.cmm
./app
```

Or manually without `gmm`:

```bash
ghc -c -no-hs-main 01_hello_print.cmm -o app.o
cc app.o -o app -lm -lc
./app
```

---

## Table of Examples

| # | File | Topic |
| --- | --- | --- |
| 1 | `01_hello_print.cmm` | Simple text printing |
| 2 | `02_variables_types.cmm` | Data types and variables |
| 3 | `03_int_arithmetic.cmm` | Integer arithmetic operations |
| 4 | `04_float_arithmetic.cmm` | Floating-point arithmetic operations |
| 5 | `05_comparisons.cmm` | Comparison operations |
| 6 | `06_bitwise_ops.cmm` | Bitwise operations |
| 7 | `07_conversions.cmm` | Type conversions |
| 8 | `08_if_else.cmm` | Conditionals |
| 9 | `09_goto_loop.cmm` | Loops via goto |
| 10 | `10_switch_case.cmm` | switch/case |
| 11 | `11_jump_direct.cmm` | Control transfer between functions (jump) |
| 12 | `12_jump_indirect.cmm` | Indirect function jump |
| 13 | `13_foreign_c.cmm` | External C function calls |
| 14 | `14_import.cmm` | Forward declaration of external functions |
| 15 | `15_section_data_bss.cmm` | Static data and reserved memory definitions |
| 16 | `16_prim_math.cmm` | Advanced built-in math primitives |
| 17 | `17_string_handling.cmm` | String manipulation |
| 18 | `18_file_io.cmm` | File input/output |
| 19 | `19_dynamic_memory.cmm` | Dynamic memory allocation |

---

## 1. Simple Text Printing

The simplest possible program. `moutf` is a shortcut from `stdc--.h` for `printf` from the C standard library — supporting the same format specifiers (`%d`, `%f`, `%s`...). Every Cmm program starts from a `main()` function and must end with `mexit(0)` rather than `return (0)` — using `return` at the end of `main()` may crash the program after successful execution due to GHC's internal RTS return mechanisms not present in standalone Cmm programs.

## 2. Data Types and Variables

Unlike languages like C, Cmm has no generic `int` type. Every type is explicitly declared by its bit-width:

| Type | Size | Description |
| --- | --- | --- |
| `bits8` | 8 bits | Small integer or character |
| `bits16` | 16 bits | Medium integer |
| `bits32` | 32 bits | Standard integer (like `int` in C) |
| `bits64` | 64 bits | Large integer |
| `float32` | 32 bits | Single-precision floating-point |
| `float64` | 64 bits | Double-precision floating-point (like `double`) |
| `gcptr` | Machine native | Managed garbage-collection pointer |
| `W_` | 32 or 64 bits | Native CPU word size (automatically adjusts per architecture) |

Variable types must be declared first, followed by value assignment on a separate line — combined declaration and initialization is not supported.

## 3. Integer Arithmetic Operations

Addition, subtraction, multiplication, division, and modulo work directly and safely on integer types (`bits8`...`bits64`) using standard operator symbols (`+ - * / %`).

## 4. Floating-Point Arithmetic Operations

⚠️ **Important Warning**: Using traditional symbols `+ - * /` directly on `float64` is unsafe — GHC's Native Code Generator (NCG) backend can emit invalid instructions mixing integer and floating-point registers, leading to incorrect results or crashes. This is a documented GHC compiler issue, not user code error.

The safe solution is using explicit built-in primitives in `%name(...)` format:

| Standard Symbol | Safe Primitive Alternative |
| --- | --- |
| `a + b` | `%fadd(a, b)` |
| `a - b` | `%fsub(a, b)` |
| `a * b` | `%fmul(a, b)` |
| `a / b` | `%fquot(a, b)` |

## 5. Comparison Operations

Standard comparison operators: `<`, `>`, `<=`, `>=`, `==`, `!=`. These are used directly inside conditional blocks (`if`).

## 6. Bitwise Operations

| Symbol | Operation |
| --- | --- |
| `&` | AND |
| `|` | OR |
| `^` | XOR |
| `~` | NOT |
| `<<` | Shift Left |
| `>>` | Shift Right |

## 7. Type Conversions

Cmm does not perform implicit type conversions — every conversion must be explicit using built-in primitives in `%name(...)` format:

| Primitive | Description |
| --- | --- |
| `%zx8/16/32/64(x)` | Zero-extend |
| `%sx8/16/32/64(x)` | Sign-extend |
| `%lobits8/16/32/64(x)` | Truncate to lower bit width |
| `%i2f32/64(x)` | Convert integer to float |
| `%f2i8/16/32/64(x)` | Convert float to integer |
| `%f2f32/64(x)` | Convert between float32 and float64 precision |

## 8. Conditionals

The `if`/`else` structure operates identically to C logic and supports nesting.

## 9. Loops via goto

Cmm provides no high-level loop constructs such as `for` or `while` — loops are implemented manually using labels (`label:`) and `goto`. Jump operations are local to the current function scope and cannot jump to labels defined in external functions.

## 10. switch/case

A multi-way branch structure similar to C's `switch`, including `default` case handling.

## 11. Control Transfer Between Functions (jump)

Unlike a standard C function call, `jump` in Cmm is a **tail call with no return** — there is no automatic "call and resume" mechanism without a full GHC runtime environment (RTS). Any code following a `jump` instruction within the same function scope will never execute. To build multi-step execution flows, each step must be an exported function (`export`), passing shared data through static global memory (`section "bss"`) instead of return values.

## 12. Indirect Function Jump

Cmm can store function entry addresses as regular scalar values in memory or registers and dynamically jump to them later (analogous to C function pointers).

## 13. External C Function Calls

`foreign "C"` is the primitive mechanism to invoke any external function — whether standard C libraries (`libc`) or foreign modules exporting C-compatible ABIs (such as C++ or Rust).

## 14. Forward Declaration of External Functions

`import "C" function_name;` is an optional forward declaration (similar to `extern` in C) placed prior to invoking foreign calls.

## 15. Static Data and Reserved Memory Definitions

* `section "data" { ... }`: Initialized static data structures (e.g., string literals).
* `section "bss" { ... }`: Uninitialized static memory buffers used for global mutable state, accessible across functions via their memory addresses.

## 16. Advanced Built-in Math Primitives

Using the `prim %name(...)` syntax, high-performance compiler-native math primitives are exposed directly without external library linkage: `sqrt64f`, `sin64f`, `cos64f`, `tan64f`, `sinh64f`, `cosh64f`, `tanh64f`, `asin64f`, `acos64f`, `atan64f`, `log64f`, `log1p64f`, `exp64f`, `expM164f`, `fabs64f`, `pow64f` (and their 32-bit counterparts using `32f`). These primitives are safe and bypass the native floating-point compiler bug described in Section 4.

## 17. String Manipulation

`stdc--.h` wraps standard `<string.h>` operations: `mstrlen`, `mstrcpy`, `mstrcat`, `mstrcmp`, and related utilities.

## 18. File Input/Output

Wrappers for `<stdio.h>` stream I/O: `mopen`, `mclose`, `fmout` (string output), `mread`, `mwrite`, and stream management routines.

## 19. Dynamic Memory Allocation

Wrappers for standard `<stdlib.h>` dynamic allocation: `mnew` (`malloc`), `mnewz` (`calloc`), `mresize` (`realloc`), and `mfree`.

## 20. FFI Call Shortcuts

`ffi` serves as a streamlined macro alias for `foreign "C"`, reducing syntax verbosity across sequential FFI invocations.

## 21. Safe Floating-Point Math via String Conversion Pattern

An alternative, highly stable design pattern for floating-point calculations: converting numerical values to strings, passing them to external helper functions that handle calculations completely outside Cmm runtime context, and writing results back to target memory pointers.

⚠️ **Notice**: Certain multi-argument variadic helper routines require an explicit terminating `0` argument. Omitting the null terminator will not cause a syntax error, but will cause the routine to read garbage memory addresses, producing subtle invalid calculations without compiler warnings.

---

## Quick Reference: Tool Selection Guide

| Requirement | Recommended Solution |
| --- | --- |
| I/O Operations | `moutf` / `min` via `stdc--.h` |
| Integer Calculations | Native operators `+ - * / %` |
| Basic Float Calculations | Primitives `%fadd`, `%fsub`, `%fmul`, `%fquot` |
| Advanced Math (Trig, Root, Log) | Native primitives `prim %name64f(...)` |
| Maximum Safety Float Operations | `cmmath.h` string conversion pattern |
| Looping / Control Flow | `goto` + `label:` (intra-function scope) |
| Inter-function Execution | Direct / Indirect `jump` (non-returning) |
| External Library Binding | `foreign "C"` or `ffi` shorthand |

---

# Part 2: Cmm Math API Documentation (`cmmath.h`)

## Overview

`cmmath.h` provides a comprehensive, unified mathematical toolkit designed specifically for C-- programs running via Foreign Function Interface (FFI). All API procedures operate on double-precision floating-point numbers (`double`/`float64`) and employ the standard `m` prefix convention. This abstraction layer enables robust mathematical processing while avoiding known native compiler limitations associated with direct Cmm floating-point operations.

---
[example](https://github.com/DASKR515/C-minus-minus/tree/main/example-cmmath/example)

## Basic Arithmetic Operations

### `madd`

* **Description:** Performs double-precision floating-point addition ($result = a + b$).
* **Parameters:**
* `result`: Pointer to memory destination where the sum is written.
* `a`: First operand (augend).
* `b`: Second operand (addend).


* **Behavior:** Adheres to IEEE 754 standard addition semantics, correctly handling standard floating-point infinities and NaN conditions.

### `msub`

* **Description:** Performs double-precision floating-point subtraction ($result = a - b$).
* **Parameters:**
* `result`: Pointer to memory destination where the difference is written.
* `a`: Minuend operand.
* `b`: Subtrahend operand.



### `mmul`

* **Description:** Performs double-precision floating-point multiplication ($result = a \times b$).
* **Parameters:**
* `result`: Pointer to memory destination where the product is written.
* `a`: Multiplicand operand.
* `b`: Multiplier operand.



### `mfdiv`

* **Description:** Performs double-precision floating-point division ($result = a / b$).
* **Parameters:**
* `result`: Pointer to memory destination where the quotient is written.
* `a`: Dividend operand.
* `b`: Divisor operand.


* **Behavior:** Division by zero returns signed infinity ($\pm\infty$). Zero divided by zero evaluates to NaN.

---

## Trigonometric Functions

### `msin`

* **Description:** Computes the trigonometric sine of an angle given in radians ($result = \sin(a)$).
* **Parameters:** `result` (destination pointer), `a` (angle in radians).
* **Output Range:** $[-1.0, 1.0]$.

### `mcos`

* **Description:** Computes the trigonometric cosine of an angle given in radians ($result = \cos(a)$).
* **Parameters:** `result` (destination pointer), `a` (angle in radians).
* **Output Range:** $[-1.0, 1.0]$.

### `mtan`

* **Description:** Computes the trigonometric tangent of an angle given in radians ($result = \tan(a)$).
* **Parameters:** `result` (destination pointer), `a` (angle in radians).
* **Notes:** Asymptotic spikes occur near odd multiples of $\pi/2$.

### `masin`

* **Description:** Computes the principal inverse sine (arcsine) in radians ($result = \arcsin(a)$).
* **Parameters:** `result` (destination pointer), `a` (input value).
* **Domain:** $[-1.0, 1.0]$. Values outside this domain yield NaN. Range: $[-\pi/2, \pi/2]$.

### `macos`

* **Description:** Computes the principal inverse cosine (arccosine) in radians ($result = \arccos(a)$).
* **Parameters:** `result` (destination pointer), `a` (input value).
* **Domain:** $[-1.0, 1.0]$. Values outside this domain yield NaN. Range: $[0, \pi]$.

### `matan`

* **Description:** Computes the principal inverse tangent (arctangent) in radians ($result = \arctan(a)$).
* **Parameters:** `result` (destination pointer), `a` (input value).
* **Output Range:** $[-\pi/2, \pi/2]$.

### `matan2`

* **Description:** Computes the two-argument inverse tangent of $a/b$, using sign information to determine the correct quadrant ($result = \arctan2(a, b)$).
* **Parameters:** `result` (destination pointer), `a` (y-coordinate), `b` (x-coordinate).
* **Output Range:** $[-\pi, \pi]$.

---

## Hyperbolic Functions

### `msinh`

* **Description:** Computes the hyperbolic sine ($result = \sinh(a) = \frac{e^a - e^{-a}}{2}$).
* **Parameters:** `result` (destination pointer), `a` (input value).

### `mcosh`

* **Description:** Computes the hyperbolic cosine ($result = \cosh(a) = \frac{e^a + e^{-a}}{2}$).
* **Parameters:** `result` (destination pointer), `a` (input value). Output value is always $\ge 1.0$.

### `mtanh`

* **Description:** Computes the hyperbolic tangent ($result = \tanh(a) = \frac{\sinh(a)}{\cosh(a)}$).
* **Parameters:** `result` (destination pointer), `a` (input value). Output Range: $[-1.0, 1.0]$.

### `masinh`

* **Description:** Computes the inverse hyperbolic sine ($result = \text{asinh}(a) = \ln(a + \sqrt{a^2 + 1})$).
* **Parameters:** `result` (destination pointer), `a` (input value defined over all real numbers).

### `macosh`

* **Description:** Computes the inverse hyperbolic cosine ($result = \text{acosh}(a) = \ln(a + \sqrt{a^2 - 1})$).
* **Parameters:** `result` (destination pointer), `a` (input value $\ge 1.0$). Values $< 1.0$ yield NaN.

### `matanh`

* **Description:** Computes the inverse hyperbolic tangent ($result = \text{atanh}(a) = \frac{1}{2} \ln\left(\frac{1 + a}{1 - a}\right)$).
* **Parameters:** `result` (destination pointer), `a` (input value in open interval $(-1.0, 1.0)$).

---

## Exponential & Logarithmic Functions

| Function | Mathematical Definition | Domain / Notes |
| --- | --- | --- |
| `mexp` | $e^a$ | Base-$e$ exponential. Can overflow to $\infty$. |
| `mexp2` | $2^a$ | Base-$2$ exponential. |
| `mexpm1` | $e^a - 1$ | High-precision exponential minus one for small $a \approx 0$. |
| `mlog` | $\ln(a)$ | Natural logarithm ($a > 0$). |
| `mlog10` | $\log_{10}(a)$ | Base-$10$ logarithm ($a > 0$). |
| `mlog2` | $\log_2(a)$ | Base-$2$ logarithm ($a > 0$). |
| `mlog1p` | $\ln(1 + a)$ | High-precision natural logarithm of $1+a$ for $a > -1$. |
| `mlogb` | $\text{exponent}(a)$ | Extracts floating-point unbiased exponent as float. |
| `milogb` | $\text{exponent}(a)$ | Extracts floating-point unbiased exponent as integer. |

---

## Power & Root Functions

* **`mpow(result, a, b)`**: Computes $a^b$. Negative base with fractional exponent returns NaN.
* **`msqrt(result, a)`**: Computes square root $\sqrt{a}$ for $a \ge 0$.
* **`mcbrt(result, a)`**: Computes cube root $\sqrt[3]{a}$ for all real numbers.
* **`mhypot(result, a, b)`**: Computes hypotenuse $\sqrt{a^2 + b^2}$ with underflow/overflow protection.

---

## Special Mathematical Functions

* **`mtgamma(result, a)`**: Computes the Gamma function $\Gamma(a)$. Has poles at non-positive integers.
* **`mlgamma(result, a)`**: Computes natural logarithm of absolute Gamma function $\ln\|\Gamma(a)\|$.
* **`merf(result, a)`**: Computes standard Error function $\text{erf}(a) \in [-1.0, 1.0]$.
* **`merfc(result, a)`**: Computes Complementary Error function $\text{erfc}(a) = 1 - \text{erf}(a) \in [0.0, 2.0]$.

---

## Absolute, Sign & Remainder Utilities

* **`mfabs(result, a)`**: Absolute value $\|a\|$.
* **`mcopysign(result, a, b)`**: Returns value with magnitude of $a$ and sign of $b$.
* **`mfmod(result, a, b)`**: Floating-point remainder $a - \text{trunc}(a/b) \times b$.
* **`mremainder(result, a, b)`**: IEEE 754 remainder $a - \text{round}(a/b) \times b$.
* **`mremquo(result, quo, a, b)`**: Computes remainder and populates integer quotient `quo`.
* **`mfmax(result, a, b)` / `mfmin(result, a, b)**`: Returns maximum or minimum value (handles NaN gracefully).
* **`mfdim(result, a, b)`**: Positive difference $\max(a - b, 0)$.

---

## Floating-Point Decomposition & Scaling

* **`mfrexp(result, exp, a)`**: Decomposes float into normalized fraction $[0.5, 1.0)$ and integer exponent.
* **`mmodf(frac, integer, a)`**: Splits float into integral and fractional components.
* **`mldexp(result, a, n)`** / **`mscalbn(result, a, n)`**: Computes $a \times 2^n$ using integer exponent $n$.
* **`mscalbln(result, a, n)`**: Computes $a \times 2^n$ using long integer exponent $n$.
* **`mnextafter(result, a, b)`**: Returns next representable floating-point value from $a$ in direction of $b$.

---

# Part 3: C-- Standard Library API Reference (`stdc--.h`)

This API reference documents the full collection of standard C library wrappers available in `stdc--.h` for the C-- (Cmm) programming language.

## 1. `<stdio.h>` — Input, Output & File Stream Management

### Standard Formatting and Output

| Standard C Routine | Custom C-- Macro | Functional Description |
| --- | --- | --- |
| `fdopen(0, "r\0")` | `mget_stdin()` | Opens stdin file stream pointer for low-level reading. |
| `puts(str)` | `mout(str)` | Writes string to `stdout` with appended newline. |
| `printf(...)` | `moutf(...)` | Formatted string output to standard output (`stdout`). |
| `putchar(c)` | `moutc(c)` | Writes single character to `stdout`. |
| `fputs(str, stream)` | `fmout(stream, str)` | Writes raw string buffer to designated output stream. |
| `fputc(c, stream)` | `fmoutc(c, stream)` | Writes single character to designated stream. |
| `fprintf(stream, ...)` | `fmoutf(stream, ...)` | Formatted output to specified file stream. |
| `sprintf(buf, ...)` | `smoutf(buf, ...)` | Formats string into memory buffer. |
| `snprintf(buf, n, ...)` | `snmoutf(buf, n, ...)` | Bounded formatted string output to buffer (overflow safe). |
| `vprintf(fmt, args)` | `vmoutf(fmt, args)` | Formatted output to `stdout` via `va_list` argument. |
| `vfprintf(stream, fmt, args)` | `vfmoutf(stream, fmt, args)` | Formatted output to file stream via `va_list`. |
| `vsprintf(buf, fmt, args)` | `vsmoutf(buf, fmt, args)` | Formatted output to memory buffer via `va_list`. |
| `vsnprintf(buf, n, fmt, args)` | `vsnmoutf(buf, n, fmt, args)` | Bounded formatted output to memory buffer via `va_list`. |

### Standard Parsing and Input

| Standard C Routine | Custom C-- Macro | Functional Description |
| --- | --- | --- |
| `scanf(...)` | `min(...)` | Reads and parses formatted input from `stdin`. |
| `getchar()` | `minc()` | Fetches next character from `stdin`. |
| `fgetc(stream)` | `fminc(stream)` | Fetches next character from targeted file stream. |
| `fgets(buf, n, stream)` | `fmin(buf, n, stream)` | Reads line or up to $n-1$ bytes into memory buffer. |
| `fscanf(stream, ...)` | `fminf(stream, ...)` | Parses formatted data directly from stream. |
| `sscanf(buf, ...)` | `sminf(buf, ...)` | Parses formatted data directly from string buffer. |
| `vscanf(fmt, args)` | `vminf(fmt, args)` | Variadic formatted input parsing from `stdin`. |
| `vfscanf(stream, fmt, args)` | `vfminf(stream, fmt, args)` | Variadic formatted input parsing from file stream. |
| `vsscanf(buf, fmt, args)` | `vsminf(buf, fmt, args)` | Variadic formatted input parsing from string buffer. |
| `ungetc(c, stream)` | `mungetc(c, stream)` | Pushes character back into stream input buffer. |

### File System I/O Operations

| Standard C Routine | Custom C-- Macro | Functional Description |
| --- | --- | --- |
| `fopen(path, mode)` | `mopen(path, mode)` | Opens file at specified path with access mode. |
| `freopen(path, mode, stream)` | `mreopen(path, mode, stream)` | Reassigns existing stream to new file/mode. |
| `fclose(stream)` | `mclose(stream)` | Closes file stream and flushes associated buffers. |
| `fflush(stream)` | `mflush(stream)` | Flushes unwritten buffered data to underlying storage. |
| `remove(path)` | `mremove(path)` | Deletes specified system file. |
| `rename(old, new_)` | `mrename(old, new_)` | Renames or moves system file target. |
| `tmpfile()` | `mtmpfile()` | Creates temporary binary file automatically removed on close. |
| `tmpnam(buf)` | `mtmpname(buf)` | Generates unique temporary filename string. |
| `fread(ptr, size, cnt, stream)` | `mread(ptr, size, cnt, stream)` | Reads raw binary blocks into memory buffer. |
| `fwrite(ptr, size, cnt, stream)` | `mwrite(ptr, size, cnt, stream)` | Writes raw binary blocks to output stream. |

### Positioning and Error Handling

| Standard C Routine | Custom C-- Macro | Functional Description |
| --- | --- | --- |
| `fseek(stream, off, whence)` | `mseek(stream, off, whence)` | Repositions stream offset indicator. |
| `ftell(stream)` | `mtell(stream)` | Obtains current byte position within file stream. |
| `rewind(stream)` | `mrewind(stream)` | Resets file offset indicator back to start of file. |
| `fgetpos(stream, pos)` | `mgetpos(stream, pos)` | Captures current file position into `fpos_t` structure. |
| `fsetpos(stream, pos)` | `msetpos(stream, pos)` | Restores file position from saved `fpos_t` object. |
| `feof(stream)` | `meof(stream)` | Evaluates End-of-File indicator status for stream. |
| `ferror(stream)` | `merror(stream)` | Evaluates error indicator status for stream. |
| `clearerr(stream)` | `mclearerr(stream)` | Clears both error and EOF status indicators for stream. |
| `perror(str)` | `mperror(str)` | Outputs descriptive error text for current system `errno`. |
| `setbuf(stream, buf)` | `msetbuf(stream, buf)` | Assigns external stream buffer or disables buffering. |
| `setvbuf(stream, buf, mode, size)` | `msetvbuf(stream, buf, mode, size)` | Configures stream buffering mode and capacity. |

---

## 2. `<stdlib.h>` — Dynamic Memory, Process Control & Utilities

### Memory Allocation

| Standard C Routine | Custom C-- Macro | Functional Description |
| --- | --- | --- |
| `malloc(size)` | `mnew(size)` | Allocates uninitialized memory block of designated byte size. |
| `calloc(cnt, size)` | `mnewz(cnt, size)` | Allocates zero-initialized memory block for array elements. |
| `realloc(ptr, size)` | `mresize(ptr, size)` | Resizes memory allocation block preserving existing data. |
| `free(ptr)` | `mfree(ptr)` | Releases memory block back to host operating system. |

### Process Management

| Standard C Routine | Custom C-- Macro | Functional Description |
| --- | --- | --- |
| `exit(code)` | `mexit(code)` | Terminates calling process normally with cleanup handlers. |
| `abort()` | `mab_ort()` | Terminates process abnormally via `SIGABRT` signal. |
| `quick_exit(code)` | `mquick_exit(code)` | Terminates process immediately bypassing `atexit` routines. |
| `atexit(func)` | `matexit(func)` | Registers function callback to run upon normal process exit. |
| `at_quick_exit(func)` | `mat_quick_exit(func)` | Registers callback function for `quick_exit` events. |

### Conversions & Numeric Parsing

| Standard C Routine | Custom C-- Macro | Functional Description |
| --- | --- | --- |
| `atoi(str)` | `mtoi(str)` | Converts ASCII string to 32-bit integer. |
| `atol(str)` | `mtol(str)` | Converts ASCII string to `long` integer. |
| `atoll(str)` | `mtoll(str)` | Converts ASCII string to `long long` integer. |
| `atof(str)` | `mtof(str)` | Converts ASCII string to double-precision float. |
| `strtol(str, end, base)` | `mstrtol(str, end, base)` | Parses string to `long` with radix and error reporting. |
| `strtoul(str, end, base)` | `mstrtoul(str, end, base)` | Parses string to `unsigned long` with radix control. |
| `strtoll(str, end, base)` | `mstrtoll(str, end, base)` | Parses string to signed `long long` integer. |
| `strtoull(str, end, base)` | `mstrtoull(str, end, base)` | Parses string to `unsigned long long` integer. |
| `strtof(str, end)` | `mstrtof(str, end)` | Parses string to single-precision floating-point. |
| `strtod(str, end)` | `mstrtod(str, end)` | Parses string to double-precision floating-point. |
| `strtold(str, end)` | `mstrtold(str, end)` | Parses string to extended-precision `long double`. |

### Randomization, Searching & Sorting

| Standard C Routine | Custom C-- Macro | Functional Description |
| --- | --- | --- |
| `rand()` | `mrand()` | Generates pseudo-random integer up to `RAND_MAX`. |
| `srand(seed)` | `msrand(seed)` | Initializes pseudo-random number generator seed. |
| `qsort(base, cnt, size, cmp)` | `mqsort(base, cnt, size, cmp)` | Sorts array using Quick Sort algorithm. |
| `bsearch(key, base, cnt, size, cmp)` | `mbsearch(key, base, cnt, size, cmp)` | Binary search over sorted array structure. |
| `abs(x)` | `mabs(x)` | Computes absolute value of integer. |
| `labs(x)` | `mlabs(x)` | Computes absolute value of long integer. |
| `llabs(x)` | `mllabs(x)` | Computes absolute value of long long integer. |
| `div(num, den)` | `mdiv(num, den)` | Computes quotient and remainder of integer division. |
| `ldiv(num, den)` | `mldiv(num, den)` | Computes quotient and remainder of long division. |
| `lldiv(num, den)` | `mlldiv(num, den)` | Computes quotient and remainder of long long division. |

---

## 3. `<string.h>` — Memory & String Operations

| Standard C Routine | Custom C-- Macro | Functional Description |
| --- | --- | --- |
| `strcpy(dst, src)` | `mstrcpy(dst, src)` | Copies null-terminated string to target buffer. |
| `strncpy(dst, src, n)` | `mstrncpy(dst, src, n)` | Copies up to $n$ characters to destination buffer. |
| `memcpy(dst, src, n)` | `mmemcpy(dst, src, n)` | Copies $n$ bytes between non-overlapping memory regions. |
| `memmove(dst, src, n)` | `mmemmove(dst, src, n)` | Copies $n$ bytes safely handling overlapping regions. |
| `strcat(dst, src)` | `mstrcat(dst, src)` | Concatenates source string onto end of destination buffer. |
| `strncat(dst, src, n)` | `mstrncat(dst, src, n)` | Appends up to $n$ characters onto destination string. |
| `strcmp(a, b)` | `mstrcmp(a, b)` | Lexicographically compares two strings (0 if identical). |
| `strncmp(a, b, n)` | `mstrncmp(a, b, n)` | Compares first $n$ characters of two strings. |
| `memcmp(a, b, n)` | `mmemcmp(a, b, n)` | Compares $n$ raw bytes of memory regions. |
| `strchr(str, c)` | `mstrchr(str, c)` | Finds first occurrence of character `c` in string. |
| `strrchr(str, c)` | `mstrrchr(str, c)` | Finds last occurrence of character `c` in string. |
| `strstr(hay, needle)` | `mstrstr(hay, needle)` | Locates first occurrence of substring in main string. |
| `strtok(str, delim)` | `mstrtok(str, delim)` | Tokenizes string based on specified delimiters. |
| `strlen(str)` | `mstrlen(str)` | Returns character count of string excluding null terminator. |
| `memset(ptr, val, n)` | `mmemset(ptr, val, n)` | Fills memory block with constant byte value. |
| `strdup(str)` | `mstrdup(str)` | Duplicates string by allocating dynamic memory. |

---

## 4. `<time.h>` — Date and Time Utilities

| Standard C Routine | Custom C-- Macro | Functional Description |
| --- | --- | --- |
| `time(t)` | `mtime(t)` | Returns current UNIX Epoch timestamp in seconds. |
| `clock()` | `mclock()` | Returns CPU time consumed by calling process. |
| `difftime(t1, t0)` | `mdifftime(t1, t0)` | Computes difference in seconds between two timestamps. |
| `mktime(tm)` | `mmktime(tm)` | Converts broken-down time structure to calendar timestamp. |
| `strftime(buf, n, fmt, tm)` | `mstrftime(buf, n, fmt, tm)` | Formats broken-down time structure into custom text buffer. |
| `gmtime(t)` | `mgmtime(t)` | Converts timestamp to UTC broken-down time structure. |
| `localtime(t)` | `mlocaltime(t)` | Converts timestamp to local time broken-down structure. |

---

## 5. `<ctype.h>` — Character Classification & Transformation

All classification functions take a character byte and return a boolean status flag (non-zero for true, 0 for false).

| Standard C Routine | Custom C-- Macro | Functional Description |
| --- | --- | --- |
| `isalpha(c)` | `misalpha(c)` | Checks if character is alphabetic (`A-Z`, `a-z`). |
| `isdigit(c)` | `misdigit(c)` | Checks if character is decimal digit (`0-9`). |
| `isalnum(c)` | `misalnum(c)` | Checks if character is alphanumeric. |
| `isspace(c)` | `misspace(c)` | Checks if character is whitespace (space, tab, newline). |
| `ispunct(c)` | `mispunct(c)` | Checks if character is punctuation symbol. |
| `isupper(c)` / `islower(c)` | `misupper(c)` / `mislower(c)` | Checks for uppercase / lowercase letter. |
| `toupper(c)` / `tolower(c)` | `mtoupper(c)` / `mtolower(c)` | Converts character to uppercase / lowercase. |

---

## 6. Multi-Threading & Synchronization (`<threads.h>`)

| Standard C Routine | Custom C-- Macro | Functional Description |
| --- | --- | --- |
| `thrd_create(t, func, arg)` | `mthrd_create(t, func, arg)` | Spawns parallel execution thread. |
| `thrd_join(t, res)` | `mthrd_join(t, res)` | Blocks until target thread terminates. |
| `thrd_detach(t)` | `mthrd_detach(t)` | Releases thread resources automatically upon exit. |
| `mtx_init(m, type)` | `mmtx_init(m, type)` | Initializes mutual exclusion lock object. |
| `mtx_lock(m)` | `mmtx_lock(m)` | Acquires mutex lock (blocks if claimed). |
| `mtx_unlock(m)` | `mmtx_unlock(m)` | Releases acquired mutex lock. |
| `cnd_init(c)` / `cnd_wait(c, m)` | `mcnd_init(c)` / `mcnd_wait(c, m)` | Condition variable initialization and blocking wait. |
| `cnd_signal(c)` / `cnd_broadcast(c)` | `mcnd_signal(c)` / `mcnd_broadcast(c)` | Signals one or all threads waiting on condition. |
```
