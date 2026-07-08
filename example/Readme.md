<div align="center">

# cmmath.h

**A floating-point math library for C-- (Cmm) built around the NCG-Isolation Architecture.**

*Safe floating-point operations for GHC Cmm by isolating the Native Code Generator (NCG) from floating-point semantics.*

</div>

---

## Overview

`cmmath.h` is a mathematical extension for **C-- (Cmm)** designed to provide reliable floating-point operations without exposing GHC's Native Code Generator (NCG) to floating-point semantics.

Instead of allowing the compiler to manipulate floating-point values directly, the library introduces an isolation layer where all floating-point computations are performed inside the C runtime (`libc`), while Cmm and the NCG only move raw memory.

This design is called the **NCG-Isolation Architecture**.

---

# Why does this library exist?

The primary issue is not mathematical functions themselves.

The real problem appears when GHC's Native Code Generator attempts to interpret floating-point values inside Cmm and emit SSE instructions.

This can lead to invalid assembly generation and assembler errors.

Instead of relying on NCG to understand floating-point values, `cmmath.h` completely removes floating-point semantics from NCG's responsibility.

---

# The NCG-Isolation Architecture

The architecture follows one simple rule:

> **NCG never performs floating-point computation. It only moves memory.**

The computation is delegated to the C runtime.

```
String Literal
      │
      ▼
 testx()
      │
      ▼
 IEEE-754 Raw Bits
      │
      ▼
 bits64 Memory
      │
      ▼
 libc Math Functions
      │
      ▼
 bits64 Result
      │
      ▼
 float64 Read
      │
      ▼
 printf()
```

The compiler never needs to understand floating-point registers during the mathematical operation.

---

# The Core Component: `testx`

`testx` is the foundation of the entire library.

Instead of declaring floating-point constants directly inside Cmm, values are first represented as strings.

Example:

```c
"0.5"
```

`testx` converts the string into raw IEEE-754 binary representation inside a trusted C environment.

The produced value is written into memory as `bits64`.

From the perspective of NCG, this is simply raw memory.

It has no knowledge that the stored data represents a floating-point number.

This completely avoids the faulty floating-point code generation path.

---

# Architecture Walkthrough

## Step 1

Reserve memory.

```c
section "bss" { x: bits64; }
section "bss" { res: bits64; }
```

Both variables are ordinary 64-bit memory locations.

No floating-point semantics are involved.

---

## Step 2

Store the floating-point value.

```c
foreign "C" testx(x, s1 "ptr");
```

The string

```
"0.5"
```

is converted into IEEE-754 binary and written into `x`.

The conversion occurs entirely inside C.

---

## Step 3

Perform the math.

```c
macos(res, x);
```

The C runtime performs the mathematical operation.

NCG only passes memory addresses.

It never manipulates floating-point registers.

---

## Step 4

Read the result.

```c
r = float64[res];
```

After computation finishes, the stored bits are read back as `float64`.

Since arithmetic has already completed, this becomes a simple memory read.

---

## Step 5

Print the value.

```c
moutf(fmt "ptr", r);
```

The resulting value can now be printed normally.

---

# Example

```c
#include "Cmm.h"
#include "stdc--.h"
#include "cmmath.h"

section "bss" { x: bits64; }
section "bss" { res: bits64; }

section "data" { s1: bits8[] "0.5\0"; }
section "data" { fmt: bits8[] "result = %f\n\0"; }

export main;

main()
{
    float64 r;

    foreign "C" testx(x, s1 "ptr");

    macos(res, x);

    r = float64[res];

    moutf(fmt "ptr", r);

    mexit(0);
}
```

---

# Technical Notes

The supplied C header acts as a **Stable Math Engine**.

Every wrapped math function accepts memory pointers rather than native floating-point parameters.

This creates a **Memory-Mapped Arithmetic Interface**, where:

- Cmm provides memory.
- NCG moves memory.
- libc performs computation.
- Results return as raw IEEE-754 data.

Because every operation follows this model, the architecture remains consistent across all mathematical functions.

---

# Design Philosophy

The library intentionally separates responsibilities.

| Component | Responsibility |
|-----------|----------------|
| Cmm | Program logic |
| NCG | Memory movement |
| libc | Floating-point computation |
| cmmath.h | Bridge between both worlds |

This isolation minimizes compiler involvement in floating-point handling while keeping mathematical operations predictable.

---

# Summary

`cmmath.h` is not simply a collection of math wrappers.

It is an architectural layer that isolates floating-point computation from GHC's Native Code Generator by treating floating-point values as opaque IEEE-754 memory during compilation and delegating all computation to the C runtime.

---
