<div align="center">

<img src="icon.png" alt="C-- (Cmm) Language Reference" width="140">

# C-- (Cmm) Language Reference

*A comprehensive reference for GHC's native C-- (Cmm) intermediate language.*

![License](https://img.shields.io/badge/license-BSD--2--Clause-blue)
![Language](https://img.shields.io/badge/language-C--%20(Cmm)-blue)
![Compiler](https://img.shields.io/badge/compiler-GHC-orange)
![Status](https://img.shields.io/badge/status-active-brightgreen)

**Documentation • Examples • Compiler Usage • FFI • Language Reference**

</div>

---

# Overview

**C-- (Cmm) Language Reference** is a community-driven documentation project dedicated to **C-- (Cmm)**, the native intermediate representation (IR) used internally by the **Glasgow Haskell Compiler (GHC)**.

The goal of this repository is to provide a centralized reference covering the language syntax, compiler usage, practical examples, memory layout, control flow, foreign function interfaces (FFI), and interoperability with native C libraries.

This project is **not** a compiler, framework, runtime environment, or replacement for GHC. Instead, it serves as a complete reference for developers interested in learning, understanding, and writing Cmm programs.

---

# Features

- Comprehensive C-- (Cmm) language reference
- Syntax documentation
- Practical examples
- GHC compilation guide
- FFI documentation
- Native C ABI examples
- Data sections
- Procedures
- Labels and control flow
- Memory layout examples
- Pointer operations
- Community-maintained documentation

---

# Getting Started

## Installing GHC

Cmm programs are compiled directly using the **Glasgow Haskell Compiler (GHC)**.

Download GHC:

https://www.haskell.org/ghc/

---

## Compiling Cmm

Compile a standalone Cmm program:

```bash
ghc -no-hs-main main.cmm -o app
```

The `-no-hs-main` option tells GHC not to link the default Haskell runtime entry point, allowing the Cmm program to define its own `main()` function.

---

# Hello World

```C
#include "Cmm.h"

section "data" {
    msg: bits8[] "Hello, World!\0";
}

export main;

main() {
    foreign "C" puts(msg "ptr");
    return (0);
}
```

This program declares a string inside the data section and invokes the standard C library function `puts()` using GHC's native Foreign Function Interface.

---

# Using stdc--.h

This repository also references **stdc--.h**, a lightweight helper library that simplifies writing C-- (Cmm) programs while remaining fully compatible with the native C ABI.

Repository:

https://github.com/DASKR515/stdc--.h/blob/main/stdc--.h

Without **stdc--.h**:

```C
#include "Cmm.h"

section "data" {
    msg: bits8[] "Hi Jordan\0";
}

export main;

main() {
    foreign "C" puts(msg "ptr");
    return (0);
}
```

With **stdc--.h**:

```C
#include "Cmm.h"
#include "stdc--.h"

section "data" {
    msg: bits8[] "Hi Jordan\0";
}

export main;

main() {
    mout(msg "ptr");
    return (0);
}
```

The `mout()` helper wraps the standard C `puts()` function, reducing boilerplate while keeping programs clean and readable.

**stdc--.h** is an independent open-source project created and maintained by the author of this repository. Its purpose is to provide lightweight helper functions and common declarations for C-- (Cmm) without modifying the language itself.

---

# Language Topics

This repository covers:

- Language syntax
- Sections
- Data declarations
- Procedures
- Exported symbols
- Labels
- Branch instructions
- Control flow
- Calling C functions
- Foreign Function Interface (FFI)
- Native C ABI
- Memory layouts
- Pointer operations
- Practical examples
- Compilation with GHC

---

# Useful Resources

## Glasgow Haskell Compiler (GHC)

https://www.haskell.org/ghc/

Official compiler used to compile Cmm source files.

---

## GHC User Guide

https://downloads.haskell.org/ghc/latest/docs/users_guide/

Official compiler documentation.

---

## GHC Source Code

https://gitlab.haskell.org/ghc/ghc

Official GHC source repository.

---

## stdc--.h

https://github.com/DASKR515/stdc--.h/blob/main/stdc--.h

Lightweight helper library for C-- (Cmm).

---

# Contributing

Contributions are welcome.

If you find incorrect information, missing documentation, or would like to improve examples or explanations, feel free to open an issue or submit a pull request.

---

# Project Status

This repository is actively maintained.

Documentation, examples, compiler notes, and language references will continue to expand as additional Cmm features and implementation details are explored.

---

# License

This project is licensed under the **BSD 2-Clause License**.

See the `LICENSE` file for details.

---

# Acknowledgements

- Glasgow Haskell Compiler (GHC) Project
- The Haskell Community
- Contributors to GHC and Cmm documentation

Special thanks to everyone contributing documentation, examples, and research related to the C-- (Cmm) language.
