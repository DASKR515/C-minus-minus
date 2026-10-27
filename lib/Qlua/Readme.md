# Qlua

## What is Qlua?

Qlua is a Cmm binding library for Lua 5.4.8. It provides a comprehensive interface to the Lua 5.4.8 C API (core API, auxiliary library, and standard libraries) for use from Cmm code through the same FFI pattern used by Cmmath.

## Why does it exist?

This project follows the established conventions of the Cmmath library in this environment - all floating point/math operations are routed through C wrappers because direct operations can be problematic in Cmm, and the FFI is done via `foreign "C"` macros. Qlua applies the same architectural approach to expose Lua's C API to Cmm.

## Exact Lua version supported

**Lua 5.4.8 exclusively

## Purpose of every project file

| File | Purpose |
|------|---------|
| `gmm` | Cmm build tool (existing) |
| `Qlua.h` | Public Cmm interface - macros wrapping foreign C calls to ql_* functions, constants/macros mirroring Lua API |
| `Qlua.c` | Implementation layer - C functions that call actual Lua 5.4.8 API functions, following Cmmath.c style |
| `Readme.md` | This file - introduction, usage, compilation, linking |
| `wiki.md` | Comprehensive API documentation for all exposed functions/macros/types |

## Requirements and dependencies

- **Cmm toolchain**: `gmm` (v4.0) for compiling Cmm code
- **C compiler**: gcc/clang for compiling `Qlua.c` 
- **Cmm headers**: `Cmm.h`, `stdc--.h` as used by the project style

## How the library is compiled

Compile the C implementation:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build --parallel
```
## How it is linked

When building Cmm programs with gmm, link the object/library files. Examples from the lua-ex test show:
```bash
gmm -o app main.cmm app.o build/libQlua.so
```

For Qlua, you would compile `Qlua.c` to an object and link it together with Lua 5.4.8 library and your Cmm program.

## How it is called from Cmm

Include the header and use `ql_*` macros which expand to `foreign "C" ql_*`. Follow the same pattern as cmmath:
```C
#include "Cmm.h"
#include "stdc--.h"
#include "Qlua.h"

section "bss" { L: bits64; }
section "bss" { res: bits32; }

export main;

main() {
    ql_newstate(L);
    ql_openlibs(bits64[L]);
    qlL_dostring(res, bits64[L], script_ptr);
    ql_close(bits64[L]);
    mexit(0);
}
```

Note: Following Cmmath conventions, values are passed via addresses; the C wrappers handle reading/writing with memcpy. Check wiki.md for exact calling conventions for each function.

## How to initialize and close a Lua state

```C
section "bss" { L: bits64; }
ql_newstate(L);           /* Creates new Lua state via luaL_newstate */
ql_openlibs(bits64[L]);   /* Open all standard libraries */
...
ql_close(bits64[L]);      /* Close state */
```

Or use `qlL_newstate` if you need auxlib's version. Both are exposed.

## How to load and execute Lua code

```C
section "bss" { res: bits32; }
section "data" { code: bits8[] "return 1+2\0"; }

/* Load string */
qlL_loadstring(res, bits64[L], code "ptr");
/* Execute (pcall) */
ql_pcall(res2, bits64[L], 0, QLUA_MULTRET, 0);
```

Or use convenience macros: `qlL_dostring`, `qlL_dofile` (exposed as wrappers).

## How to pass values between Cmm and Lua

- **Push to stack**: `ql_pushnumber`, `ql_pushinteger`, `ql_pushstring`, `ql_pushboolean`, `ql_pushnil`, `ql_newtable`, etc.
- **Read from stack**: `ql_tonumber`, `ql_tointeger`, `ql_toboolean`, `ql_tolstring`, `ql_type`, etc. Results written to pointer parameters.
- **Tables/globals**: `ql_getglobal`, `ql_setglobal`, `ql_getfield`, `ql_setfield`, `ql_gettable`, `ql_settable`

## How to handle errors

Lua C API uses status codes: `QLUA_OK`, `QLUA_ERRRUN`, `QLUA_ERRSYNTAX`, `QLUA_ERRMEM`, `QLUA_ERRERR`, `QLUA_ERRFILE`. Functions like `ql_pcall` return status via result pointer. On error, error message is typically on stack - retrieve with `ql_tolstring`.

## Current limitations and compatibility requirements

- **Cmm calling convention**: Follows same pattern as Cmmath - results go to pointer params, values passed as pointers/addresses. Exact compatibility depends on Cmm toolchain behavior.
- **Variadic functions**: Some Lua APIs are variadic (e.g., `lua_pushfstring`, `luaL_error`, `lua_gc`). Wrappers attempt to handle common cases; complex variadic usage may require additional adaptation.
- **Function pointers/callbacks**: `lua_CFunction`, hooks, allocators etc. can be passed as pointers - representation depends on Cmm interop.
- **Structs**: `lua_Debug`, `luaL_Buffer`, `luaL_Reg`, `luaL_Stream` are opaque/passed as pointers in general; full struct manipulation from Cmm may be limited.
- **Some macros**: Pure Lua macros (like `lua_pop`, `lua_newtable` as simple macros) are reimplemented as macros in Qlua.h where feasible.
- **Platform/ABI**: Uses Lua 5.4.8 as built in the environment; types like `lua_Integer`/`lua_Number` follow luaconf.h settings.
