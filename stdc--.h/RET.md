# `ret.h` — Continuation-Passing Style for Bare-Metal Cmm

`ret.h` is a lightweight, header-only library for **Cmm (C--)** programs compiled with the **GHC Native Code Generator (NCG)** in environments where the **GHC Runtime System (RTS) is not linked**.

The library provides an explicit mechanism for function calls and returns based on **Continuation-Passing Style (CPS)**. Instead of relying on conventional C-style `return` semantics, functions receive an explicit continuation address and return by performing a direct `jump` to that continuation.

The result is a simple control-flow model designed specifically for **RTS-free Cmm programs**.

---

## 1. Motivation

Hand-written Cmm is normally processed through GHC's backend infrastructure. However, when Cmm is compiled and linked without the Haskell RTS, conventional return mechanisms can become problematic.

A traditional function return may rely on stack and calling-convention assumptions associated with the code generated for the GHC runtime environment.

For example:

```cmm
return (value);
```

may not provide the control-flow behavior expected by a standalone, RTS-free Cmm program.

`ret.h` avoids this model by making the continuation explicit.

Instead of:

```text
function()
    |
    +--> return value
```

the control flow becomes:

```text
function(ret_addr)
    |
    +--> jump ret_addr(value)
```

The return destination is therefore treated as an explicit part of the function's interface.

---

# 2. Core Design

The basic model used by `ret.h` is:

```text
Caller
  |
  | continuation address
  v
Function
  |
  | jump continuation(result)
  v
Continuation
```

Each function receives a continuation, commonly represented as:

```cmm
W_ ret_addr
```

The function does not need to construct a conventional stack-based return path. Instead, it transfers control directly to the continuation.

This gives the programmer explicit control over the next point of execution.

---

# 3. Function Declarations

## `fnc(name)`

Defines a function that receives an implicit continuation address.

Conceptually:

```cmm
fnc(name)
```

represents a function with the following logical interface:

```text
name(ret_addr)
```

The continuation is supplied by the caller.

---

## `fnc_a(name, ...)`

Defines a function with additional explicit arguments.

Example:

```cmm
fnc_a(square, bits32 x)
```

Conceptually:

```text
square(ret_addr, x)
```

The continuation remains implicit in the macro-generated function interface, while the listed arguments are explicit.

---

# 4. Returning from a Function

`ret.h` provides several return macros.

## `ret_v()`

Returns without a value:

```cmm
ret_v();
```

Conceptually expands to:

```cmm
jump ret_addr ();
```

---

## `ret(value)`

Returns one value:

```cmm
ret(result);
```

Conceptually:

```cmm
jump ret_addr (result);
```

---

## `ret2(value1, value2)`

Returns two values simultaneously:

```cmm
ret2(sum, diff);
```

Conceptually:

```cmm
jump ret_addr (sum, diff);
```

This allows multiple results to be passed directly to the continuation without requiring a heap object or an explicit result structure.

---

## `ret3(value1, value2, value3)`

Returns three values:

```cmm
ret3(a, b, c);
```

Conceptually:

```cmm
jump ret_addr (a, b, c);
```

The continuation receives all three values directly.

---

# 5. Calling Functions

The library also provides call macros that explicitly specify the continuation.

## `mall_v(fn, cont)`

Calls a function without additional arguments:

```cmm
mall_v(function, continuation);
```

The logical control flow is:

```text
function(continuation)
```

---

## `mall(fn, cont, ...)`

Calls a function with additional arguments:

```cmm
mall(function, continuation, arg1, arg2);
```

Conceptually:

```text
function(continuation, arg1, arg2)
```

The important distinction is that the caller explicitly determines where the called function should continue.

---

# 6. Example 1 — Basic Return

This example calculates the square of a number and sends the result to a continuation.

```cmm
#include "Cmm.h"
#include "stdc--.h"
#include "ret.h"

section "data" {
    fmt: bits8[] "Square result = %d\n\0";
}

export main;

fnc_a(square, bits32 x) {
    bits32 res;

    res = x * x;

    ret(res);
}

fnc_a(on_square_done, bits32 result) {
    moutf(fmt "ptr", result);
    mexit(0);
}

main() {
    mall(square, on_square_done, 12);
}
```

### Control Flow

The execution can be viewed as:

```text
main
  |
  | mall(square, on_square_done, 12)
  v
square
  |
  | res = 12 * 12
  |
  | ret(res)
  v
on_square_done(144)
  |
  | print result
  v
exit
```

The important point is that `square` does not use a conventional `return`. It explicitly transfers control to `on_square_done`.

---

# 7. Example 2 — Multiple Return Values

`ret.h` can pass multiple values directly to a continuation.

This example calculates both the sum and the difference of two numbers.

```cmm
#include "Cmm.h"
#include "stdc--.h"
#include "ret.h"

section "data" {
    fmt: bits8[] "Sum = %d, Diff = %d\n\0";
}

export main;

fnc_a(calc_math, bits32 a, bits32 b) {
    bits32 sum;
    bits32 diff;

    sum = a + b;
    diff = a - b;

    ret2(sum, diff);
}

fnc_a(on_math_done, bits32 s, bits32 d) {
    moutf(fmt "ptr", s, d);
    mexit(0);
}

main() {
    mall(calc_math, on_math_done, 50, 20);
}
```

The resulting control flow is:

```text
main
  |
  | calc_math(50, 20)
  v
calc_math
  |
  | sum  = 70
  | diff = 30
  |
  | ret2(sum, diff)
  v
on_math_done(70, 30)
```

No intermediate result structure is required.

---

# 8. Example 3 — Chained Continuations

Continuations can also be chained to construct larger control-flow sequences.

```cmm
#include "Cmm.h"
#include "stdc--.h"
#include "ret.h"

section "data" {
    fmt: bits8[] "Step 1 = %d, Step 2 = %d\n\0";
}

export main;

fnc_a(step2, bits32 val) {
    bits32 res;

    res = val * 2;

    ret(res);
}

fnc_a(on_step2, bits32 final_val) {
    moutf(fmt "ptr", 10, final_val);
    mexit(0);
}

fnc_a(step1, bits32 val) {
    mall(step2, on_step2, val + 5);
}

main() {
    mall(step1, on_step2, 5);
}
```

The control flow is:

```text
main
  |
  v
step1(5)
  |
  | val + 5
  v
step2(10)
  |
  | 10 * 2
  v
on_step2(20)
  |
  v
exit
```

Each stage explicitly specifies the next continuation.

---

# 9. Why CPS?

The primary purpose of this design is to make control flow explicit.

Traditional function execution can be represented as:

```text
call
  |
  v
function
  |
  v
return
  |
  v
caller
```

With `ret.h`, the model becomes:

```text
call
  |
  v
function(ret_addr)
  |
  | jump ret_addr(...)
  v
continuation
```

The continuation is therefore not an implicit property of a conventional return instruction. It is an explicit value passed to the function.

This is particularly useful when building standalone Cmm programs where the programmer wants to avoid depending on the Haskell RTS control-flow environment.

---

# 10. Stack Behavior

A continuation-based call chain does not inherently require a new conventional call frame for every logical step.

For example:

```text
main
  |
  v
step1
  |
  v
step2
  |
  v
step3
  |
  v
step4
```

can be represented as a sequence of jumps between continuations.

When the generated code and calling convention preserve this structure as intended, the logical depth of the continuation chain does not automatically imply equivalent growth of the native call stack.

This makes CPS useful for constructing long-running control-flow chains.

However, the actual stack behavior ultimately depends on the generated machine code, register allocation, ABI requirements, local allocations, and the surrounding Cmm code.

---

# 11. Runtime Independence

`ret.h` is designed for a specific environment:

```text
Cmm
  |
  v
GHC Cmm Front-End
  |
  v
GHC NCG
  |
  v
Native Object Code
  |
  v
System Linker
```

The design does **not** require the Haskell RTS to implement the continuation mechanism.

The continuation address is handled explicitly by the generated Cmm control flow.

This makes the header suitable for projects that intentionally use GHC's Cmm compiler and NCG while avoiding the normal Haskell runtime environment.

---

# 12. Design Goals

`ret.h` is built around several simple goals:

### Explicit control flow

The destination of a function's result is explicitly provided by the caller.

### RTS-independent returns

The mechanism does not depend on the normal Haskell RTS return path.

### Direct continuation jumps

Returns are represented using Cmm `jump` operations.

### Multiple return values

`ret2` and `ret3` allow several values to be passed directly to a continuation.

### Header-only implementation

The library is distributed as a single header and does not require a separate runtime library.

### Minimal abstraction

The macros remain close to the underlying Cmm control-flow primitives.

---

# 13. Important Notes

`ret.h` is **not a general replacement for the Cmm calling convention**.

It is specifically intended for carefully controlled, RTS-free Cmm programs.

The programmer must still respect the requirements of:

* GHC's Cmm syntax
* GHC NCG
* the target architecture
* the platform ABI
* register allocation
* data representation
* memory management
* external FFI calls

In particular, using `ret.h` does not automatically make arbitrary Cmm code safe. Code that mixes conventional calls, foreign functions, stack-sensitive operations, or incompatible calling conventions must still be designed according to the target platform and GHC backend behavior.

---

# 14. Summary

`ret.h` provides a small CPS-oriented interface for standalone Cmm programs:

```text
fnc_a()      → define a continuation-based function
mall()       → call a function with a continuation
ret()        → return one value
ret2()       → return two values
ret3()        → return three values
ret_v()      → return without a value
```

The central idea is simple:

```cmm
ret(value);
```

means:

```cmm
jump ret_addr (value);
```

while:

```cmm
mall(function, continuation, args...);
```

establishes the continuation explicitly.

The resulting programming model is:

```text
        explicit continuation
                |
                v
        +----------------+
        |    function    |
        +----------------+
                |
                | jump
                v
        +----------------+
        |  continuation  |
        +----------------+
```

For RTS-free Cmm, this provides a compact way to express function sequencing and result passing while keeping the continuation mechanism explicit and close to the underlying Cmm control-flow primitives.
