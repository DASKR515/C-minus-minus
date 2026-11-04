# rts.h
by ALEXIS
A header-only wrapper that makes the GHC RTS and STG layer easy to use from
standalone Cmm (C--) programs. It is the companion of `stdc--.h`: where
`stdc--.h` wraps the C standard library, `rts.h` wraps what the GHC runtime
system exports.

- Target: GHC 9.12.4
- Build: `gmm -opt 2 -rts main.cmm -o app`
- Usage: `#include "Cmm.h"`, `#include "stdc--.h"`, `#include "rts.h"`

## Idea

Calling an RTS function from Cmm normally needs two things every time:

```c
import "C" isDoubleNaN;
(nan) = foreign "C" isDoubleNaN(d);
```

`rts.h` does the import once and gives each function a short macro:

```c
(nan) = r_isnan(d);
```

Pointer arguments get their `"ptr"` hint inside the macro, so you do not write it.

## Goals

1. Remove FFI boilerplate for RTS functions.
2. Collect the RTS and STG names in one place, with their signatures in comments.
3. Be honest about what is verified. Every entry is tagged:
   - **[V]** the name exists in `libHSrts-1.0.3.a` (checked with `nm` on the real GHC 9.12.4 install), or it linked and ran correctly.
   - **[N]** not found in the RTS library. It may live in another GHC library or may not exist.
   - **[U]** not checked.
4. Provide pure-Cmm helpers (IEEE 754 bit macros) that need no linking at all.

## What is in it

| Section | Content |
|---|---|
| 1. Imports | C functions exported by the RTS (float classification, encode/decode, bit tricks, atomics, time, messages, lifecycle, GC) |
| 2. `r_` macros | Short `foreign "C"` wrappers for section 1 |
| 3. `R_` macros | IEEE 754 helpers for double and float, and bitcast through memory |
| 4. STG symbols | Primops (`stg_*zh`), generic apply, GC entry points, info tables, closures |
| 5. `K_` macros | Cmm calling convention (`call` / `jump`) for STG procedures |

See `wiki.md` for every function and macro.

## Symbol check

`rts.h` was compared with `nm` output of `libHSrts-1.0.3.a` (1537 functions, 280 data symbols).
Result: 430 of 443 imported names exist. The 13 that do not are the `isFloat*` family,
`rintDouble`, `rintFloat`, `__2Int_encodeDouble`, and `isDouble*`. The `isDouble*` functions
still linked and ran for you, so `gmm -rts` gets them from another GHC library.
Names removed because they do not exist: `performMinorGC`, `stg_myThreadIdzh`, `stg_ap_0_info`,
`stg_gc_enter_1`, `stg_gc_fun`, `stg_MSG_NULL_closure`, `rtsSupportsBoundThreads`.
`stg_doubleToWord64zh` also does not exist. The real name is `stg_castDoubleToWord64zh`.

## Findings so far

These came out of testing on GHC 9.12.4 and are the reason the header looks the way it does.

- `-rts` is required to link RTS C functions. Without it, `isDoubleInfinite` gives `undefined reference`.
- `__decodeDouble_Int64` has the signature `StgInt f(StgInt64 *mantissa, StgDouble d)`.
  It writes the mantissa through the pointer and **returns the exponent**.
  Passing a second pointer is silently ignored, so the exponent looks like 0.
- `x = f(y);` is not a valid call in Cmm. Use `(x) = foreign "C" f(y);`.
  The parser reports `parse error on input`, not a missing symbol.
- Stg primop names such as `stg_doubleToWord64zh` cannot be used like C functions.
  Read the bits through memory instead (`R_BITS2D`, `R_D2BITS`).
- A data section must be one item per block:
  `section "data" { name: bits64 value; }`. The form `bits64[5] { ... }` is a parse error.
- GHC can panic with `LocalReg's live-in to graph`. Initialising every local variable
  at the top of the function made it go away in our tests.
- Do not pass `float64` to variadic C functions such as `printf`. Print the raw bits.
- Unused `import` lines cost nothing. Only functions you actually call must link.
- Primops and closures need a Haskell execution context (`BaseReg`, `Hp`, `Sp`).
  Importing them is safe. Calling them from a bare `main` is not.

## Tested

`main.cmm` in this folder was built and run. These all gave correct results:
`r_isnan`, `r_isinf`, `r_isfinite`, `r_isdenorm`, `r_isnegzero`, `r_dec64`,
`r_enc_i`, and the `R_D_*` bit macros, on 3.7, +inf, NaN, -0.0 and the smallest denormal.

Not yet tested: everything tagged **[U]** (bit tricks, atomics, time, lifecycle,
all `stg_*` names, all `K_` macros).

## Known limits

- Signatures are from GHC headers and memory. `nm` proves a name exists, not what its arguments are.
- The RTS has roughly 500 `stg_*` symbols. `rts.h` covers the main ones, not all of them.
- Cmm integer comparisons (`<`, `>`) are believed to be unsigned. Unconfirmed here.
- Symbol names can change between GHC versions.

