# Qlua API Wiki (Lua 5.4.8)

This documentation describes the actual implementation in `Qlua.h` and `Qlua.c`. All APIs target Lua 5.4.8 exclusively.

## Conventions (following Cmmath)

- Cmm calls go through macros: `ql_<name>(...)` expands to `foreign "C" ql_<name>(...)`
- C implementation functions are named `ql_<name>` in `Qlua.c`
- Results are typically written to the first pointer parameter (result/out parameter) via `memcpy`, similar to Cmmath style
- Pointers/addresses are passed from Cmm; C side reads/writes as appropriate
- NULL checks are performed where reasonable

## Constants and Types

### Version
- `LUA_VERSION_5_4_8`, `QLUA_LUA_VERSION_MAJOR/MINOR/RELEASE`
- Verify against actual Lua version at runtime if needed via `ql_version`

### Basic Types
- `QLUA_TNONE`, `QLUA_TNIL`, `QLUA_TBOOLEAN`, `QLUA_TLIGHTUSERDATA`, `QLUA_TNUMBER`, `QLUA_TSTRING`, `QLUA_TTABLE`, `QLUA_TFUNCTION`, `QLUA_TUSERDATA`, `QLUA_TTHREAD`, `QLUA_NUMTYPES`

### Status
- `QLUA_OK`, `QLUA_YIELD`, `QLUA_ERRRUN`, `QLUA_ERRSYNTAX`, `QLUA_ERRMEM`, `QLUA_ERRERR`, `QLUA_ERRFILE`

### Registry/Pseudo-indices
- `QLUA_MULTRET`, `QLUA_REGISTRYINDEX`, `QLUA_RIDX_MAINTHREAD`, `QLUA_RIDX_GLOBALS`

### GC
- `QLUA_GCSTOP`, `QLUA_GCRESTART`, `QLUA_GCCOLLECT`, `QLUA_GCCOUNT`, `QLUA_GCCOUNTB`, `QLUA_GCSTEP`, `QLUA_GCSETPAUSE`, `QLUA_GCSETSTEPMUL`, `QLUA_GCISRUNNING`, `QLUA_GCGEN`, `QLUA_GCINC`

### Comparison/Arith
- `QLUA_OPEQ`, `QLUA_OPLT`, `QLUA_OPLE`
- `QLUA_OPADD`..`QLUA_OPBNOT` (see Qlua.h)

### Debug hooks
- `QLUA_HOOKCALL`, `QLUA_HOOKRET`, `QLUA_HOOKLINE`, `QLUA_HOOKCOUNT`, `QLUA_HOOKTAILCALL`, `QLUA_MASK*`

### Auxlib
- `QLUA_GNAME`, `QLUA_LOADED_TABLE`, `QLUA_PRELOAD_TABLE`, `QLUA_FILEHANDLE`, `QLUA_NOREF`, `QLUA_REFNIL`

---

## API Reference

### State manipulation

| Function | Signature (Cmm macro) | Description | Notes |
|----------|------------------------|-------------|-------|
| `ql_newstate` | `ql_newstate(result_ptr)` | Creates new state via `luaL_newstate()` (wrapper uses `luaL_newstate` in impl). | Writes `lua_State*` to result. |
| `qlL_newstate` | `qlL_newstate(result_ptr)` | Same as above via auxlib. | Exposed separately. |
| `ql_close` | `ql_close(L)` | Closes state. | NULL-safe. |
| `ql_newthread` | `ql_newthread(result_ptr, L)` | Creates new thread. | Returns thread state ptr. |
| `ql_closethread` | `ql_closethread(result_ptr, L, from)` | Closes thread. | Returns status. |
| `ql_atpanic` | `ql_atpanic(result_ptr, L, panicf)` | Sets panic function; returns old panic function as pointer. | C function pointer passed as-is. |
| `ql_version` | `ql_version(result_ptr, L)` | Gets Lua version number. | Writes `lua_Number`. |
| `ql_status` | `ql_status(result_ptr, L)` | Gets thread status. | Writes int. |
| `ql_isyieldable_res` / `ql_isyieldable` | varies | Check if yieldable. | Both forms exposed. |

### Stack manipulation

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `ql_absindex` | `ql_absindex(result_ptr, L, idx)` | Convert to absolute index. | Writes int. |
| `ql_gettop` | `ql_gettop(result_ptr, L)` | Get stack top. | Writes int. |
| `ql_settop` | `ql_settop(L, idx)` | Set stack top. | Void. |
| `ql_pushvalue` | `ql_pushvalue(L, idx)` | Push copy of element. | Void. |
| `ql_rotate` | `ql_rotate(L, idx, n)` | Rotate elements. | Void. |
| `ql_copy` | `ql_copy(L, fromidx, toidx)` | Copy element. | Void. |
| `ql_checkstack` | `ql_checkstack(result_ptr, L, n)` | Ensure stack space. | Writes int (success). |
| `ql_xmove` | `ql_xmove(from, to, n)` | Move values between threads. | Void. |
| `ql_pop` | Macro: `ql_pop(L,n) ql_settop(L, (-(n))-1)` | Pop elements. | Macro in Qlua.h. |

### Access (stack -> C)

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `ql_isnumber` | `ql_isnumber(result_ptr, L, idx)` | Check if number. | int result. |
| `ql_isstring` | `ql_isstring(result_ptr, L, idx)` | Check if string. | int result. |
| `ql_iscfunction` | `ql_iscfunction(result_ptr, L, idx)` | Check if C function. | int result. |
| `ql_isinteger` | `ql_isinteger(result_ptr, L, idx)` | Check if integer (Lua 5.3+). | int result. |
| `ql_isuserdata` | `ql_isuserdata(result_ptr, L, idx)` | Check if userdata. | int result. |
| `ql_type` | `ql_type(result_ptr, L, idx)` | Get type. | int result. |
| `ql_typename` | `ql_typename(result_ptr, L, tp)` | Get type name. | Writes const char* pointer. |
| `ql_tonumberx` | `ql_tonumberx(result_ptr, L, idx, isnum_ptr)` | Convert to number. | Writes lua_Number. Pass isnum_ptr or NULL as appropriate. |
| `ql_tointegerx` | `ql_tointegerx(result_ptr, L, idx, isnum_ptr)` | Convert to integer. | Writes lua_Integer. |
| `ql_tonumber` | `ql_tonumber(result_ptr, L, idx)` | Convert to number (no isnum). | Convenience wrapper exposed. |
| `ql_tointeger` | `ql_tointeger(result_ptr, L, idx)` | Convert to integer. | Convenience wrapper exposed. |
| `ql_toboolean` | `ql_toboolean(result_ptr, L, idx)` | Convert to boolean. | int result. |
| `ql_tolstring` | `ql_tolstring(result_ptr, L, idx, len_ptr)` | Get string pointer and length. | Writes const char*. |
| `ql_tostring` | `ql_tostring(result_ptr, L, idx)` | Get string (macro-style). | Writes const char*. |
| `ql_rawlen` | `ql_rawlen(result_ptr, L, idx)` | Get raw length. | Writes lua_Unsigned. |
| `ql_tocfunction` | `ql_tocfunction(result_ptr, L, idx)` | Get C function pointer. | Writes function pointer. |
| `ql_touserdata` | `ql_touserdata(result_ptr, L, idx)` | Get userdata pointer. | Writes void*. |
| `ql_tothread` | `ql_tothread(result_ptr, L, idx)` | Get thread. | Writes lua_State*. |
| `ql_topointer` | `ql_topointer(result_ptr, L, idx)` | Get pointer representation. | Writes const void*. |
| Type helpers | `ql_isfunction`, `ql_istable`, `ql_isnil`, `ql_isboolean`, `ql_isthread`, `ql_isnone`, `ql_isnoneornil`, `ql_islightuserdata` | All write int results. | Exposed. |

### Push (C -> stack)

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `ql_pushnil` | `ql_pushnil(L)` | Push nil. | Void. |
| `ql_pushnumber` | `ql_pushnumber(L, n)` | Push number (lua_Number). | Void. |
| `ql_pushinteger` | `ql_pushinteger(L, n)` | Push integer (lua_Integer). | Void. |
| `ql_pushlstring` | `ql_pushlstring(result_ptr, L, s, len)` | Push string with length. | Returns pointer to string in Lua stack (written to result). |
| `ql_pushstring` | `ql_pushstring(result_ptr, L, s)` | Push string. | Returns pointer written to result. |
| `ql_pushfstring` | `ql_pushfstring(result_ptr, L, fmt, ...)` | Push formatted string. | Variadic - simplified handling in wrapper; complex cases may need care. |
| `ql_pushcclosure` | `ql_pushcclosure(L, fn, n)` | Push C closure. | fn is lua_CFunction pointer. |
| `ql_pushcfunction` | Macro to `ql_pushcclosure(L,f,0)` | Push C function. | Macro. |
| `ql_pushboolean` | `ql_pushboolean(L, b)` | Push boolean. | Void. |
| `ql_pushlightuserdata` | `ql_pushlightuserdata(L, p)` | Push light userdata. | Void. |
| `ql_pushthread` | `ql_pushthread(result_ptr, L)` | Push thread; returns status. | Writes int. |
| `ql_pushglobaltable` | `ql_pushglobaltable(L)` | Push global table. | Void. |

### Get/Set (tables/globals)

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `ql_getglobal` | `ql_getglobal(result_ptr, L, name)` | Get global; returns type. | Writes int type. |
| `ql_setglobal` | `ql_setglobal(L, name)` | Set global. | Void. |
| `ql_gettable` | `ql_gettable(result_ptr, L, idx)` | Get table value; returns type. | Writes int. |
| `ql_settable` | `ql_settable(L, idx)` | Set table value. | Void. |
| `ql_getfield` | `ql_getfield(result_ptr, L, idx, k)` | Get field; returns type. | Writes int. |
| `ql_setfield` | `ql_setfield(L, idx, k)` | Set field. | Void. |
| `ql_geti` | `ql_geti(result_ptr, L, idx, n)` | Get integer index; returns type. | Writes int. |
| `ql_seti` | `ql_seti(L, idx, n)` | Set integer index. | Void. |
| `ql_rawget` | `ql_rawget(result_ptr, L, idx)` | Raw get; returns type. | Writes int. |
| `ql_rawset` | `ql_rawset(L, idx)` | Raw set. | Void. |
| `ql_rawgeti` | `ql_rawgeti(result_ptr, L, idx, n)` | Raw get integer index. | Writes int. |
| `ql_rawseti` | `ql_rawseti(L, idx, n)` | Raw set integer index. | Void. |
| `ql_rawgetp` | `ql_rawgetp(result_ptr, L, idx, p)` | Raw get by pointer key. | Writes int. |
| `ql_rawsetp` | `ql_rawsetp(L, idx, p)` | Raw set by pointer key. | Void. |
| `ql_createtable` | `ql_createtable(L, narr, nrec)` | Create table. | Void. |
| `ql_newtable` | Macro `ql_newtable(L) ql_createtable(L,0,0)` | Create empty table. | Macro. |
| `ql_newuserdatauv` | `ql_newuserdatauv(result_ptr, L, sz, nuvalue)` | Create userdata with user values. | Writes void*. |
| `ql_newuserdata` | `ql_newuserdata(result_ptr, L, sz)` | Create userdata (compat). | Writes void*. |
| `ql_getmetatable` | `ql_getmetatable(result_ptr, L, objindex)` | Get metatable. | Writes int. |
| `ql_setmetatable` | `ql_setmetatable(result_ptr, L, objindex)` | Set metatable. | Writes int. |
| `ql_getiuservalue` | `ql_getiuservalue(result_ptr, L, idx, n)` | Get uservalue. | Writes int type. |
| `ql_setiuservalue` | `ql_setiuservalue(result_ptr, L, idx, n)` | Set uservalue. | Writes int. |
| `ql_getuservalue` | `ql_getuservalue(result_ptr, L, idx)` | Get uservalue (compat). | Writes int. |
| `ql_setuservalue` | `ql_setuservalue(result_ptr, L, idx)` | Set uservalue (compat). | Writes int. |

### Comparison and arithmetic

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `ql_arith` | `ql_arith(L, op)` | Perform arithmetic/comparison ops. | Void. Use QLUA_OP* constants. |
| `ql_rawequal` | `ql_rawequal(result_ptr, L, idx1, idx2)` | Raw equality. | Writes int. |
| `ql_compare` | `ql_compare(result_ptr, L, idx1, idx2, op)` | Compare. | Writes int. Use QLUA_OPEQ/LT/LE. |

### Calls and loading

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `ql_call` | `ql_call(L, nargs, nresults)` | Call function (unprotected). | Void - errors longjmp in C API; use with caution. |
| `ql_callk` | `ql_callk(L, nargs, nresults, ctx, k)` | Call with continuation. | Void. |
| `ql_pcall` | `ql_pcall(result_ptr, L, nargs, nresults, errfunc)` | Protected call. | Writes status int. |
| `ql_pcallk` | `ql_pcallk(result_ptr, L, nargs, nresults, errfunc, ctx, k)` | Protected call with continuation. | Writes status int. |
| `ql_load` | `ql_load(result_ptr, L, reader, dt, chunkname, mode)` | Load chunk. | Writes status int. Reader is function pointer. |
| `ql_dump` | `ql_dump(result_ptr, L, writer, data, strip)` | Dump chunk. | Writes status int. Writer is function pointer. |

### Coroutines

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `ql_yield` | `ql_yield(result_ptr, L, nresults)` | Yield. | Writes int status. |
| `ql_yieldk` | `ql_yieldk(result_ptr, L, nresults, ctx, k)` | Yield with continuation. | Writes int status. |
| `ql_resume` | `ql_resume(result_ptr, L, from, narg, nres_ptr)` | Resume coroutine. | Writes status int. |
| `ql_isyieldable` / `ql_isyieldable_res` | varies | Check yieldability. | Both forms exposed. |

### Garbage collection

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `ql_gc` | `ql_gc(result_ptr, L, what)` | GC operation. | Writes int result. |
| `ql_gc_int` | `ql_gc_int(result_ptr, L, what, arg)` | GC operation with int arg. | Writes int result. |

### Miscellaneous

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `ql_error` | `ql_error(result_ptr, L)` | Raise error. | Writes int (doesn't return normally in general). |
| `ql_next` | `ql_next(result_ptr, L, idx)` | Next table pair. | Writes int. |
| `ql_concat` | `ql_concat(L, n)` | Concatenate. | Void. |
| `ql_len` | `ql_len(L, idx)` | Get length (operator #). | Void. |
| `ql_stringtonumber` | `ql_stringtonumber(result_ptr, L, s)` | Convert string to number and push. | Writes size_t/length-like result. |
| `ql_getallocf` | `ql_getallocf(result_ptr, L, ud_ptr)` | Get allocator. | Writes allocator function pointer. |
| `ql_setallocf` | `ql_setallocf(L, f, ud)` | Set allocator. | Void. |
| `ql_toclose` | `ql_toclose(L, idx)` | Mark to-be-closed. | Void (Lua 5.4). |
| `ql_closeslot` | `ql_closeslot(L, idx)` | Close slot. | Void (Lua 5.4). |
| `ql_setwarnf` | `ql_setwarnf(L, f, ud)` | Set warning function. | Void (Lua 5.4). |
| `ql_warning` | `ql_warning(L, msg, tocont)` | Issue warning. | Void (Lua 5.4). |

### Debug API

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `ql_getstack` | `ql_getstack(result_ptr, L, level, ar_ptr)` | Get stack info. | Writes int. `ar_ptr` points to `lua_Debug` struct. |
| `ql_getinfo` | `ql_getinfo(result_ptr, L, what, ar_ptr)` | Get info. | Writes int. |
| `ql_getlocal` | `ql_getlocal(result_ptr, L, ar_ptr, n)` | Get local. | Writes const char*. |
| `ql_setlocal` | `ql_setlocal(result_ptr, L, ar_ptr, n)` | Set local. | Writes const char*. |
| `ql_getupvalue` | `ql_getupvalue(result_ptr, L, funcindex, n)` | Get upvalue. | Writes const char*. |
| `ql_setupvalue` | `ql_setupvalue(result_ptr, L, funcindex, n)` | Set upvalue. | Writes const char*. |
| `ql_upvalueid` | `ql_upvalueid(result_ptr, L, fidx, n)` | Get upvalue id. | Writes void*. |
| `ql_upvaluejoin` | `ql_upvaluejoin(L, fidx1, n1, fidx2, n2)` | Join upvalues. | Void. |
| `ql_sethook` | `ql_sethook(L, func, mask, count)` | Set hook. | Void. Hook is function pointer. |
| `ql_gethook` | `ql_gethook(result_ptr, L)` | Get hook. | Writes function pointer. |
| `ql_gethookmask` | `ql_gethookmask(result_ptr, L)` | Get hook mask. | Writes int. |
| `ql_gethookcount` | `ql_gethookcount(result_ptr, L)` | Get hook count. | Writes int. |
| `ql_setcstacklimit` | `ql_setcstacklimit(result_ptr, L, limit)` | Set C stack limit. | Writes int (Lua 5.4). |

### Auxiliary library

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `qlL_checkversion` / `qlL_checkversion_aux` | `qlL_checkversion(L)` | Check version. | Void (may longjmp on mismatch). |
| `qlL_getmetafield` | `qlL_getmetafield(result_ptr, L, obj, e)` | Get metafield. | Writes int. |
| `qlL_callmeta` | `qlL_callmeta(result_ptr, L, obj, e)` | Call metafield. | Writes int. |
| `qlL_tolstring` | `qlL_tolstring(result_ptr, L, idx, len_ptr)` | Convert to string with debug info. | Writes const char*. |
| `qlL_argerror` | `qlL_argerror(result_ptr, L, arg, extramsg)` | Argument error. | Writes int (longjmp). |
| `qlL_typeerror` | `qlL_typeerror(result_ptr, L, arg, tname)` | Type error. | Writes int (longjmp). |
| `qlL_checklstring` | `qlL_checklstring(result_ptr, L, arg, l_ptr)` | Check string. | Writes const char*. |
| `qlL_optlstring` | `qlL_optlstring(result_ptr, L, arg, def, l_ptr)` | Optional string. | Writes const char*. |
| `qlL_checknumber` | `qlL_checknumber(result_ptr, L, arg)` | Check number. | Writes lua_Number. |
| `qlL_optnumber` | `qlL_optnumber(result_ptr, L, arg, def)` | Optional number. | Writes lua_Number. |
| `qlL_checkinteger` | `qlL_checkinteger(result_ptr, L, arg)` | Check integer. | Writes lua_Integer. |
| `qlL_optinteger` | `qlL_optinteger(result_ptr, L, arg, def)` | Optional integer. | Writes lua_Integer. |
| `qlL_checkstack_aux` | `qlL_checkstack_aux(L, sz, msg)` | Check stack with message. | Void. |
| `qlL_checktype` | `qlL_checktype(L, arg, t)` | Check type. | Void. |
| `qlL_checkany` | `qlL_checkany(L, arg)` | Check any. | Void. |
| `qlL_newmetatable` | `qlL_newmetatable(result_ptr, L, tname)` | Create metatable. | Writes int. |
| `qlL_setmetatable` | `qlL_setmetatable(L, tname)` | Set metatable. | Void. |
| `qlL_testudata` | `qlL_testudata(result_ptr, L, ud, tname)` | Test userdata. | Writes void*. |
| `qlL_checkudata` | `qlL_checkudata(result_ptr, L, ud, tname)` | Check userdata. | Writes void*. |
| `qlL_where` | `qlL_where(L, lvl)` | Push error location. | Void. |
| `qlL_error` | `qlL_error(result_ptr, L, fmt)` | Error. | Writes int (longjmp). Variadic form simplified. |
| `qlL_checkoption` | `qlL_checkoption(result_ptr, L, arg, def, lst_ptr)` | Check option. | Writes int. lst_ptr is array of const char*. |
| `qlL_fileresult` | `qlL_fileresult(result_ptr, L, stat, fname)` | File result. | Writes int. |
| `qlL_execresult` | `qlL_execresult(result_ptr, L, stat)` | Exec result. | Writes int. |
| `qlL_ref` | `qlL_ref(result_ptr, L, t)` | Create reference. | Writes int ref. |
| `qlL_unref` | `qlL_unref(L, t, ref)` | Release reference. | Void. |
| `qlL_loadfilex` | `qlL_loadfilex(result_ptr, L, filename, mode)` | Load file. | Writes status int. |
| `qlL_loadfile` | `qlL_loadfile(result_ptr, L, f)` | Load file. | Writes status int. |
| `qlL_loadbufferx` | `qlL_loadbufferx(result_ptr, L, buff, sz, name, mode)` | Load buffer. | Writes status int. |
| `qlL_loadbuffer` | `qlL_loadbuffer(result_ptr, L, s, sz, n)` | Load buffer. | Writes status int. |
| `qlL_loadstring` | `qlL_loadstring(result_ptr, L, s)` | Load string. | Writes status int. |
| `qlL_len` | `qlL_len(result_ptr, L, idx)` | Get length. | Writes lua_Integer. |
| `qlL_addgsub` | `qlL_addgsub(B_ptr, s, p, r)` | Add gsub to buffer. | Void. Buffer pointer. |
| `qlL_gsub` | `qlL_gsub(result_ptr, L, s, p, r)` | Global substitute. | Writes const char*. |
| `qlL_setfuncs` | `qlL_setfuncs(L, l_ptr, nup)` | Set functions. | Void. l_ptr is array of luaL_Reg. |
| `qlL_getsubtable` | `qlL_getsubtable(result_ptr, L, idx, fname)` | Get/create subtable. | Writes int. |
| `qlL_traceback` | `qlL_traceback(L, L1, msg, level)` | Traceback. | Void. |
| `qlL_requiref` | `qlL_requiref(L, modname, openf, glb)` | Require module. | Void. |
| `qlL_typename` | `qlL_typename(result_ptr, L, i)` | Get type name. | Writes const char*. |
| `qlL_checkstring` | `qlL_checkstring(result_ptr, L, n)` | Check string. | Writes const char*. |
| `qlL_optstring` | `qlL_optstring(result_ptr, L, n, d)` | Optional string. | Writes const char*. |
| `qlL_dofile` | `qlL_dofile(result_ptr, L, fn)` | Load and run file. | Writes status int. |
| `qlL_dostring` | `qlL_dostring(result_ptr, L, s)` | Load and run string. | Writes status int. |
| `qlL_getmetatable_aux` | `qlL_getmetatable_aux(result_ptr, L, n)` | Get metatable from registry. | Writes int type. |
| `qlL_newlibtable` | `qlL_newlibtable(L, l_size)` | New lib table (approx). | Void. |
| `qlL_newlib` | `qlL_newlib(L, l_size)` | New lib. | Void. |
| Macros/compat | `qlL_checkversion(L)`, `qlL_pushfail(L)` (as `ql_pushnil`), `qlL_newlibtable` style, `qlL_checkstring/optstring` | Various helpers exposed. | See Qlua.h. |

### Buffer API

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `qlL_buffinit` | `qlL_buffinit(L, B_ptr)` | Init buffer. | Void. |
| `qlL_prepbuffsize` | `qlL_prepbuffsize(result_ptr, B_ptr, sz)` | Prepare buffer. | Writes char*. |
| `qlL_addlstring` | `qlL_addlstring(B_ptr, s, l)` | Add string. | Void. |
| `qlL_addstring` | `qlL_addstring(B_ptr, s)` | Add string. | Void. |
| `qlL_addvalue` | `qlL_addvalue(B_ptr)` | Add value. | Void. |
| `qlL_pushresult` | `qlL_pushresult(B_ptr)` | Push result. | Void. |
| `qlL_pushresultsize` | `qlL_pushresultsize(B_ptr, sz)` | Push result with size. | Void. |
| `qlL_buffinitsize` | `qlL_buffinitsize(result_ptr, L, B_ptr, sz)` | Init with size. | Writes char*. |
| `qlL_addchar` | `qlL_addchar(B_ptr, c)` | Add char. | Void. |
| `qlL_addsize` | `qlL_addsize(B_ptr, s)` | Adjust size. | Void. |

### Standard libraries

| Function | Signature | Description | Notes |
|----------|-----------|-------------|-------|
| `ql_openlibs` | `ql_openlibs(L)` | Open all libraries. | Void. |
| `ql_open_base` | `ql_open_base(result_ptr, L)` | Open base lib. | Writes int. |
| `ql_open_coroutine` | `ql_open_coroutine(result_ptr, L)` | Open coroutine lib. | Writes int. |
| `ql_open_table` | `ql_open_table(result_ptr, L)` | Open table lib. | Writes int. |
| `ql_open_io` | `ql_open_io(result_ptr, L)` | Open io lib. | Writes int. |
| `ql_open_os` | `ql_open_os(result_ptr, L)` | Open os lib. | Writes int. |
| `ql_open_string` | `ql_open_string(result_ptr, L)` | Open string lib. | Writes int. |
| `ql_open_utf8` | `ql_open_utf8(result_ptr, L)` | Open utf8 lib. | Writes int. |
| `ql_open_math` | `ql_open_math(result_ptr, L)` | Open math lib. | Writes int. |
| `ql_open_debug` | `ql_open_debug(result_ptr, L)` | Open debug lib. | Writes int. |
| `ql_open_package` | `ql_open_package(result_ptr, L)` | Open package lib. | Writes int. |

### Utilities/macros
- `ql_register(L,n,f)`, `ql_replace(L,idx)`, `ql_insert(L,idx)`, `ql_remove(L,idx)`

---

## Cmm usage examples

### Initialize and run simple code
```C
#include "Cmm.h"
#include "stdc--.h"
#include "Qlua.h"

section "bss" { L: bits64; }
section "bss" { st: bits32; }
section "data" { code: bits8[] "print('hi')\0"; }

export main;

main() {
    ql_newstate(L);
    ql_openlibs(bits64[L]);
    qlL_loadstring(st, bits64[L], code "ptr");
    if (bits32[st] == QLUA_OK) {
        ql_pcall(st, bits64[L], 0, QLUA_MULTRET, 0);
    }
    ql_close(bits64[L]);
    mexit(0);
}
```

### Get global and call
```C
section "bss" { L: bits64; }
section "bss" { t: bits32; }
section "data" { name: bits8[] "print\0"; }
ql_newstate(L);
ql_openlibs(bits64[L]);
ql_getglobal(t, bits64[L], name "ptr");
ql_pushstring(rp, bits64[L], msg "ptr");
ql_pcall(st, bits64[L], 1, 0, 0);
ql_close(bits64[L]);
```

### Table manipulation
```C
ql_newtable(bits64[L]);
ql_pushstring(rp, bits64[L], key "ptr");
ql_pushinteger(bits64[L], val);
ql_settable(bits64[L], -3);
ql_getfield(t, bits64[L], -1, key2 "ptr");
```

---

## Notes on compatibility

- **Pointer passing**: Follow Cmmath pattern - pass addresses of variables/buffers from Cmm.
- **String literals**: Often passed as `label "ptr"` in examples; use same convention.
- **Function pointers**: C callbacks (C functions) must be compatible with calling convention.
- **Structs**: `lua_Debug`, `luaL_Buffer` passed as pointers; direct field access from Cmm not abstracted here - pass pointers and use C side helpers if needed.
- **Variadic**: `ql_pushfstring`, `qlL_error` have simplified wrappers; for complex variadic cases, consider writing small C helpers.
- **lua_call vs lua_pcall**: `ql_call` uses unprotected call (can longjmp) - prefer `ql_pcall` in general.
- **Lua 5.4 features**: `to-be-closed` variables (`ql_toclose`, `ql_closeslot`) and warnings (`ql_setwarnf`, `ql_warning`) are exposed as per API.

## API Coverage Status

**Implemented in Qlua.h/Qlua.c**: Comprehensive coverage of core Lua 5.4.8 API (state, stack, access/push, tables/globals, compare/arith, calls/load, coroutines, GC, misc, debug API), auxiliary library (most functions), buffer API, all standard library openers, common macros/utilities.

**Limitations/notes**:
- Some variadic functions have simplified C wrappers (they pass through to Lua API where possible, but Cmm variadic passing needs care)
- Complex struct field manipulation is not wrapped - pass struct pointers directly
- Some advanced APIs (e.g., certain debug internals, very specific edge cases) are exposed if present in headers; if symbol/export issues arise they would need adaptation
- Full coverage of every single macro form is not exhaustive where macros are pure syntactic sugar with no exported symbol
