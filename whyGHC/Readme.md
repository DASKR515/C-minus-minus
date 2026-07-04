# Cmm Math Hijacker (A Failed Methodology)

## The Problem
This project represents a technical path that I reject entirely. My goal is to write pure C-- (Cmm) code—code that is independent, low-level, and free from the bloat of Haskell’s runtime or compiler-generated abstractions.

## The Philosophical Betrayal
While attempting to implement basic math primitives, I was forced into a position where I had to hijack GHC Runtime System (RTS) symbols (`stg_`, `__`). 

The result is unacceptable: **By using this method, I am effectively acting as a manual translator for GHC.** 

When I write code this way, the resulting Cmm is indistinguishable from the intermediate code GHC generates when compiling `.hs` files. I am not writing the Cmm I want; I am writing code that mimics the compiler’s internal patterns. I despise this process. It is a direct violation of my goal to write raw, manual system code. It makes me feel like a puppet of the GHC compiler, producing output that is fundamentally "Haskell-shaped" even when I try to strip it away.

## The Path Forward: Build from Scratch
I refuse to be a slave to compiler-generated primitives anymore. 

If I need math primitives, bitwise operations, or system-level functions in the future, **I will build them from scratch.** I will implement them using raw Cmm or assembly, ensuring they are entirely decoupled from Haskell's ecosystem. I would rather endure the effort of rebuilding every single primitive from the ground up than use one more line of code that traces back to a `.hs` file or GHC's internal runtime.

## Status: NEVER AGAIN
- **Methodology**: Hijacking RTS symbols (`stg_`, `__`).
- **Verdict**: Rejected.
- **Future**: This is the last time this logic will be used. Future development will be entirely independent of GHC-generated output. I will build my own foundations, no matter the cost in development time.
