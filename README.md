<div align="center">

<img src="icon.png" alt="Cmm Runtime Logo" width="140">

# C-minus-minus (Cmm) Low-Level Runtime Environment

*A deterministic, hyper-minimalist systems programming environment engineered around GHC's native C-- Intermediate Representation.*

![Status](https://img.shields.io/badge/status-experimental-red)
![Language](https://img.shields.io/badge/language-C--%20(Cmm)-blue)
![Focus](https://img.shields.io/badge/focus-systems%20programming-black)

</div>

---

# Overview

This repository establishes a rigorous, low-level engineering environment dedicated to writing software directly in **C-- (Cmm)**, the native intermediate representation (IR) utilized within the Glasgow Haskell Compiler (GHC) backend.

The primary objective of this architecture is to expose a deterministic execution environment with zero abstraction, allowing systems engineers to interface directly with the underlying hardware architecture while maintaining a structured project layout.

Unlike conventional high-level environments that rely on complex managed runtimes, heavy standard libraries, or non-deterministic garbage collectors, this project embraces absolute control over the machine's execution flow, Application Binary Interface (ABI) bounds, and raw memory layouts.

---

# Technical Design Goals

- **Hardware-Deterministic Execution:** Elimination of hidden compiler-generated branch abstractions to ensure predictable execution paths.
- **Suppressed Runtime Footprint:** Decoupling the default language execution layer to produce ultra-small, hyper-optimized standalone binaries.
- **Explicit Memory Subsystem:** Memory allocation and pointer life cycles managed manually at the byte level with zero-cost runtime penalty.
- **Native ABI Interoperability:** Low-overhead call conventions interfacing directly with the Standard C ABI and external dynamic environments (e.g., Lua 5.4).
- **Register-Level Transparency:** Preserving direct, unhindered interaction with processor registers and low-level system hardware components.

---

# Architecture & Philosophy

### 1. Manual Memory & Data Layouts
Data structures are represented strictly as raw memory layouts. Structural hierarchy and composition are implemented via manual memory offsets. Virtual method tables (`vtable`) are intentionally avoided to eliminate indirect branching overheads and prevent instruction cache pollution.

### 2. Low-Level Control Flow
Control paths and iteration loops are driven entirely by explicit branch instructions (`goto`). This maps directly to hardware jump operations (`JMP` in assembly), maximizing pipeline predictability and eliminating overhead.

### 3. Structural Integration (`stdc--.h`)
A minimalist, custom abstraction layer designed to wrap essential system calls and platform APIs. It provides programmatic efficiency and code cleanliness without obscuring raw pointers or execution semantics.

---

# Architectural Example

[download-ghc]{https://www.haskell.org/ghc/}
The following implementation demonstrates raw data section declaration and direct external routine invocation via the native C Calling Convention:

```C
#include "Cmm.h"

section "data" {
    msg: bits8[] "Hello, World!\0";
}

export main;

main() {
    // Direct invocation of the C standard library routine using explicit pointer casting
    foreign "C" puts(msg "ptr");
    return (0);
}
```

Compilation & Toolchain Setup

To bypass the default Haskell Runtime environment and compile the raw intermediate .cmm codebase directly into a hyper-optimized standalone executable, invoke the toolchain using the -no-hs-main linking flag:
Bash
```bash
ghc -no-hs-main ./main.cmm -o app
```
    Note: Ensuring correct stack alignment and parameter passing conventions is critical when linking custom modules with external dynamic link libraries (DLLs/SO files).

Current Status & Research Focus

This framework is currently experimental and tailored for compiler exploration, low-level architecture verification, and operating system / hardware interface research.

Development is focused on enhancing the stdc--.h subsystem and strengthening FFI stability across cross-platform ABI constraints. Technical contributions, experiments, and systems-level discussions are welcome.
