/*
              --    by JABIR aka <DASKR>
              --    Qlua.h - Cmm bindings for Lua 5.4.8
              --    Names follow the 'ql' prefix convention
              --    Targets Lua 5.4.8 exclusively
              --    Follows cmmath.h style for Cmm interoperability
              --    ver <1.0.0>
*/
#pragma once
#include "Cmm.h"

/* Lua version */
#define LUA_VERSION_5_4_8 1
#define QLUA_LUA_VERSION_MAJOR 5
#define QLUA_LUA_VERSION_MINOR 4
#define QLUA_LUA_VERSION_RELEASE 8

/* Basic types - exposed as opaque where appropriate */
#define ql_void void
#define ql_int int
#define ql_size_t size_t

/* Lua basic types */
#define QLUA_TNONE (-1)
#define QLUA_TNIL 0
#define QLUA_TBOOLEAN 1
#define QLUA_TLIGHTUSERDATA 2
#define QLUA_TNUMBER 3
#define QLUA_TSTRING 4
#define QLUA_TTABLE 5
#define QLUA_TFUNCTION 6
#define QLUA_TUSERDATA 7
#define QLUA_TTHREAD 8
#define QLUA_NUMTYPES 9

/* Thread status */
#define QLUA_OK 0
#define QLUA_YIELD 1
#define QLUA_ERRRUN 2
#define QLUA_ERRSYNTAX 3
#define QLUA_ERRMEM 4
#define QLUA_ERRERR 5
#define QLUA_ERRFILE (QLUA_ERRERR + 1)

/* Lua registry pseudo-indices and options */
#define QLUA_MULTRET (-1)
#define QLUA_REGISTRYINDEX (-1000000 - 1000)
#define QLUA_RIDX_MAINTHREAD 1
#define QLUA_RIDX_GLOBALS 2

/* Garbage collection options */
#define QLUA_GCSTOP 0
#define QLUA_GCRESTART 1
#define QLUA_GCCOLLECT 2
#define QLUA_GCCOUNT 3
#define QLUA_GCCOUNTB 4
#define QLUA_GCSTEP 5
#define QLUA_GCSETPAUSE 6
#define QLUA_GCSETSTEPMUL 7
#define QLUA_GCISRUNNING 9
#define QLUA_GCGEN 10
#define QLUA_GCINC 11

/* Comparison ops */
#define QLUA_OPEQ 0
#define QLUA_OPLT 1
#define QLUA_OPLE 2

/* Arith ops */
#define QLUA_OPADD 0
#define QLUA_OPSUB 1
#define QLUA_OPMUL 2
#define QLUA_OPMOD 3
#define QLUA_OPPOW 4
#define QLUA_OPDIV 5
#define QLUA_OPIDIV 6
#define QLUA_OPBAND 7
#define QLUA_OPBOR 8
#define QLUA_OPBXOR 9
#define QLUA_OPSHL 10
#define QLUA_OPSHR 11
#define QLUA_OPUNM 12
#define QLUA_OPBNOT 13

/* Hook masks/events */
#define QLUA_HOOKCALL 0
#define QLUA_HOOKRET 1
#define QLUA_HOOKLINE 2
#define QLUA_HOOKCOUNT 3
#define QLUA_HOOKTAILCALL 4
#define QLUA_MASKCALL (1 << QLUA_HOOKCALL)
#define QLUA_MASKRET (1 << QLUA_HOOKRET)
#define QLUA_MASKLINE (1 << QLUA_HOOKLINE)
#define QLUA_MASKCOUNT (1 << QLUA_HOOKCOUNT)

/* Reference constants */
#define QLUA_NOREF (-2)
#define QLUA_REFNIL (-1)

/* Auxlib */
#define QLUA_GNAME "_G"
#define QLUA_LOADED_TABLE "_LOADED"
#define QLUA_PRELOAD_TABLE "_PRELOAD"
#define QLUA_FILEHANDLE "FILE*"

/* ========================================
 * State manipulation
 * ======================================== */
#define ql_newstate(result) foreign "C" ql_newstate(result)
#define ql_close(L) foreign "C" ql_close(L)
#define ql_newthread(result, L) foreign "C" ql_newthread(result, L)
#define ql_closethread(result, L, from) foreign "C" ql_closethread(result, L, from)
#define ql_atpanic(result, L, panicf) foreign "C" ql_atpanic(result, L, panicf)
#define ql_version(result, L) foreign "C" ql_version(result, L)
#define ql_status(result, L) foreign "C" ql_status(result, L)
#define ql_isyieldable(result, L) foreign "C" ql_isyieldable(result, L)

/* ========================================
 * Basic stack manipulation
 * ======================================== */
#define ql_absindex(result, L, idx) foreign "C" ql_absindex(result, L, idx)
#define ql_gettop(result, L) foreign "C" ql_gettop(result, L)
#define ql_settop(L, idx) foreign "C" ql_settop(L, idx)
#define ql_pushvalue(L, idx) foreign "C" ql_pushvalue(L, idx)
#define ql_rotate(L, idx, n) foreign "C" ql_rotate(L, idx, n)
#define ql_copy(L, fromidx, toidx) foreign "C" ql_copy(L, fromidx, toidx)
#define ql_checkstack(result, L, n) foreign "C" ql_checkstack(result, L, n)
#define ql_xmove(from, to, n) foreign "C" ql_xmove(from, to, n)
#define ql_pop(L, n) ql_settop(L, (-(n)) - 1)

/* ========================================
 * Access functions (stack -> C)
 * ======================================== */
#define ql_isnumber(result, L, idx) foreign "C" ql_isnumber(result, L, idx)
#define ql_isstring(result, L, idx) foreign "C" ql_isstring(result, L, idx)
#define ql_iscfunction(result, L, idx) foreign "C" ql_iscfunction(result, L, idx)
#define ql_isinteger(result, L, idx) foreign "C" ql_isinteger(result, L, idx)
#define ql_isuserdata(result, L, idx) foreign "C" ql_isuserdata(result, L, idx)
#define ql_type(result, L, idx) foreign "C" ql_type(result, L, idx)
#define ql_typename(result, L, tp) foreign "C" ql_typename(result, L, tp)
#define ql_tonumberx(result, L, idx, isnum) foreign "C" ql_tonumberx(result, L, idx, isnum)
#define ql_tointegerx(result, L, idx, isnum) foreign "C" ql_tointegerx(result, L, idx, isnum)
#define ql_tonumber(result, L, idx) foreign "C" ql_tonumber(result, L, idx)
#define ql_tointeger(result, L, idx) foreign "C" ql_tointeger(result, L, idx)
#define ql_toboolean(result, L, idx) foreign "C" ql_toboolean(result, L, idx)
#define ql_tolstring(result_ptr, L, idx, lenptr) foreign "C" ql_tolstring(result_ptr, L, idx, lenptr)
#define ql_rawlen(result, L, idx) foreign "C" ql_rawlen(result, L, idx)
#define ql_tocfunction(result, L, idx) foreign "C" ql_tocfunction(result, L, idx)
#define ql_touserdata(result, L, idx) foreign "C" ql_touserdata(result, L, idx)
#define ql_tothread(result, L, idx) foreign "C" ql_tothread(result, L, idx)
#define ql_topointer(result, L, idx) foreign "C" ql_topointer(result, L, idx)
#define ql_tostring(result_ptr, L, idx) foreign "C" ql_tostring(result_ptr, L, idx)

/* Type helpers */
#define ql_isfunction(result, L, n) foreign "C" ql_isfunction(result, L, n)
#define ql_istable(result, L, n) foreign "C" ql_istable(result, L, n)
#define ql_isnil(result, L, n) foreign "C" ql_isnil(result, L, n)
#define ql_isboolean(result, L, n) foreign "C" ql_isboolean(result, L, n)
#define ql_isthread(result, L, n) foreign "C" ql_isthread(result, L, n)
#define ql_isnone(result, L, n) foreign "C" ql_isnone(result, L, n)
#define ql_isnoneornil(result, L, n) foreign "C" ql_isnoneornil(result, L, n)
#define ql_islightuserdata(result, L, n) foreign "C" ql_islightuserdata(result, L, n)

/* ========================================
 * Push functions (C -> stack)
 * ======================================== */
#define ql_pushnil(L) foreign "C" ql_pushnil(L)
#define ql_pushnumber(L, n) foreign "C" ql_pushnumber(L, n)
#define ql_pushinteger(L, n) foreign "C" ql_pushinteger(L, n)
#define ql_pushlstring(result_ptr, L, s, len) foreign "C" ql_pushlstring(result_ptr, L, s, len)
#define ql_pushstring(result_ptr, L, s) foreign "C" ql_pushstring(result_ptr, L, s)
#define ql_pushfstring(result_ptr, L, ...) foreign "C" ql_pushfstring(result_ptr, L, __VA_ARGS__)
#define ql_pushcclosure(L, fn, n) foreign "C" ql_pushcclosure(L, fn, n)
#define ql_pushcfunction(L, f) ql_pushcclosure(L, f, 0)
#define ql_pushboolean(L, b) foreign "C" ql_pushboolean(L, b)
#define ql_pushlightuserdata(L, p) foreign "C" ql_pushlightuserdata(L, p)
#define ql_pushthread(result, L) foreign "C" ql_pushthread(result, L)
#define ql_pushglobaltable(L) foreign "C" ql_pushglobaltable(L)
#define ql_pushvalue_stack(L, idx) ql_pushvalue(L, idx)

/* ========================================
 * Get/Set table/global functions
 * ======================================== */
#define ql_getglobal(result, L, name) foreign "C" ql_getglobal(result, L, name)
#define ql_setglobal(L, name) foreign "C" ql_setglobal(L, name)
#define ql_gettable(result, L, idx) foreign "C" ql_gettable(result, L, idx)
#define ql_settable(L, idx) foreign "C" ql_settable(L, idx)
#define ql_getfield(result, L, idx, k) foreign "C" ql_getfield(result, L, idx, k)
#define ql_setfield(L, idx, k) foreign "C" ql_setfield(L, idx, k)
#define ql_geti(result, L, idx, n) foreign "C" ql_geti(result, L, idx, n)
#define ql_seti(L, idx, n) foreign "C" ql_seti(L, idx, n)
#define ql_rawget(result, L, idx) foreign "C" ql_rawget(result, L, idx)
#define ql_rawset(L, idx) foreign "C" ql_rawset(L, idx)
#define ql_rawgeti(result, L, idx, n) foreign "C" ql_rawgeti(result, L, idx, n)
#define ql_rawseti(L, idx, n) foreign "C" ql_rawseti(L, idx, n)
#define ql_rawgetp(result, L, idx, p) foreign "C" ql_rawgetp(result, L, idx, p)
#define ql_rawsetp(L, idx, p) foreign "C" ql_rawsetp(L, idx, p)
#define ql_createtable(L, narr, nrec) foreign "C" ql_createtable(L, narr, nrec)
#define ql_newtable(L) ql_createtable(L, 0, 0)
#define ql_newuserdatauv(result, L, sz, nuvalue) foreign "C" ql_newuserdatauv(result, L, sz, nuvalue)
#define ql_newuserdata(result, L, sz) foreign "C" ql_newuserdata(result, L, sz)
#define ql_getmetatable(result, L, objindex) foreign "C" ql_getmetatable(result, L, objindex)
#define ql_setmetatable(result, L, objindex) foreign "C" ql_setmetatable(result, L, objindex)
#define ql_getiuservalue(result, L, idx, n) foreign "C" ql_getiuservalue(result, L, idx, n)
#define ql_setiuservalue(result, L, idx, n) foreign "C" ql_setiuservalue(result, L, idx, n)
#define ql_getuservalue(result, L, idx) foreign "C" ql_getuservalue(result, L, idx)
#define ql_setuservalue(result, L, idx) foreign "C" ql_setuservalue(result, L, idx)

/* ========================================
 * Comparison and arithmetic
 * ======================================== */
#define ql_arith(L, op) foreign "C" ql_arith(L, op)
#define ql_rawequal(result, L, idx1, idx2) foreign "C" ql_rawequal(result, L, idx1, idx2)
#define ql_compare(result, L, idx1, idx2, op) foreign "C" ql_compare(result, L, idx1, idx2, op)

/* ========================================
 * Call functions
 * ======================================== */
#define ql_call(L, nargs, nresults) foreign "C" ql_call(L, nargs, nresults)
#define ql_callk(L, nargs, nresults, ctx, k) foreign "C" ql_callk(L, nargs, nresults, ctx, k)
#define ql_pcall(result, L, nargs, nresults, errfunc) foreign "C" ql_pcall(result, L, nargs, nresults, errfunc)
#define ql_pcallk(result, L, nargs, nresults, errfunc, ctx, k) foreign "C" ql_pcallk(result, L, nargs, nresults, errfunc, ctx, k)
#define ql_load(result, L, reader, dt, chunkname, mode) foreign "C" ql_load(result, L, reader, dt, chunkname, mode)
#define ql_dump(result, L, writer, data, strip) foreign "C" ql_dump(result, L, writer, data, strip)

/* ========================================
 * Coroutine functions
 * ======================================== */
#define ql_yield(result, L, nresults) foreign "C" ql_yield(result, L, nresults)
#define ql_yieldk(result, L, nresults, ctx, k) foreign "C" ql_yieldk(result, L, nresults, ctx, k)
#define ql_resume(result, L, from, narg, nres) foreign "C" ql_resume(result, L, from, narg, nres)
#define ql_isyieldable_res(result, L) foreign "C" ql_isyieldable_res(result, L)

/* ========================================
 * Garbage collection
 * ======================================== */
#define ql_gc(result, L, what) foreign "C" ql_gc(result, L, what)
#define ql_gc_int(result, L, what, arg) foreign "C" ql_gc_int(result, L, what, arg)

/* ========================================
 * Miscellaneous
 * ======================================== */
#define ql_error(result, L) foreign "C" ql_error(result, L)
#define ql_next(result, L, idx) foreign "C" ql_next(result, L, idx)
#define ql_concat(L, n) foreign "C" ql_concat(L, n)
#define ql_len(L, idx) foreign "C" ql_len(L, idx)
#define ql_stringtonumber(result, L, s) foreign "C" ql_stringtonumber(result, L, s)
#define ql_getallocf(result, L, ud) foreign "C" ql_getallocf(result, L, ud)
#define ql_setallocf(L, f, ud) foreign "C" ql_setallocf(L, f, ud)
#define ql_toclose(L, idx) foreign "C" ql_toclose(L, idx)
#define ql_closeslot(L, idx) foreign "C" ql_closeslot(L, idx)
#define ql_setwarnf(L, f, ud) foreign "C" ql_setwarnf(L, f, ud)
#define ql_warning(L, msg, tocont) foreign "C" ql_warning(L, msg, tocont)

/* ========================================
 * Debug API
 * ======================================== */
#define ql_getstack(result, L, level, ar) foreign "C" ql_getstack(result, L, level, ar)
#define ql_getinfo(result, L, what, ar) foreign "C" ql_getinfo(result, L, what, ar)
#define ql_getlocal(result_ptr, L, ar, n) foreign "C" ql_getlocal(result_ptr, L, ar, n)
#define ql_setlocal(result_ptr, L, ar, n) foreign "C" ql_setlocal(result_ptr, L, ar, n)
#define ql_getupvalue(result_ptr, L, funcindex, n) foreign "C" ql_getupvalue(result_ptr, L, funcindex, n)
#define ql_setupvalue(result_ptr, L, funcindex, n) foreign "C" ql_setupvalue(result_ptr, L, funcindex, n)
#define ql_upvalueid(result, L, fidx, n) foreign "C" ql_upvalueid(result, L, fidx, n)
#define ql_upvaluejoin(L, fidx1, n1, fidx2, n2) foreign "C" ql_upvaluejoin(L, fidx1, n1, fidx2, n2)
#define ql_sethook(L, func, mask, count) foreign "C" ql_sethook(L, func, mask, count)
#define ql_gethook(result, L) foreign "C" ql_gethook(result, L)
#define ql_gethookmask(result, L) foreign "C" ql_gethookmask(result, L)
#define ql_gethookcount(result, L) foreign "C" ql_gethookcount(result, L)
#define ql_setcstacklimit(result, L, limit) foreign "C" ql_setcstacklimit(result, L, limit)

/* ========================================
 * Auxiliary library
 * ======================================== */
#define qlL_checkversion(L) foreign "C" qlL_checkversion(L)
#define qlL_getmetafield(result, L, obj, e) foreign "C" qlL_getmetafield(result, L, obj, e)
#define qlL_callmeta(result, L, obj, e) foreign "C" qlL_callmeta(result, L, obj, e)
#define qlL_tolstring(result_ptr, L, idx, len) foreign "C" qlL_tolstring(result_ptr, L, idx, len)
#define qlL_argerror(result, L, arg, extramsg) foreign "C" qlL_argerror(result, L, arg, extramsg)
#define qlL_typeerror(result, L, arg, tname) foreign "C" qlL_typeerror(result, L, arg, tname)
#define qlL_checklstring(result_ptr, L, arg, l) foreign "C" qlL_checklstring(result_ptr, L, arg, l)
#define qlL_optlstring(result_ptr, L, arg, def, l) foreign "C" qlL_optlstring(result_ptr, L, arg, def, l)
#define qlL_checknumber(result, L, arg) foreign "C" qlL_checknumber(result, L, arg)
#define qlL_optnumber(result, L, arg, def) foreign "C" qlL_optnumber(result, L, arg, def)
#define qlL_checkinteger(result, L, arg) foreign "C" qlL_checkinteger(result, L, arg)
#define qlL_optinteger(result, L, arg, def) foreign "C" qlL_optinteger(result, L, arg, def)
#define qlL_checkstack_aux(L, sz, msg) foreign "C" qlL_checkstack_aux(L, sz, msg)
#define qlL_checktype(L, arg, t) foreign "C" qlL_checktype(L, arg, t)
#define qlL_checkany(L, arg) foreign "C" qlL_checkany(L, arg)
#define qlL_newmetatable(result, L, tname) foreign "C" qlL_newmetatable(result, L, tname)
#define qlL_setmetatable(L, tname) foreign "C" qlL_setmetatable(L, tname)
#define qlL_testudata(result, L, ud, tname) foreign "C" qlL_testudata(result, L, ud, tname)
#define qlL_checkudata(result, L, ud, tname) foreign "C" qlL_checkudata(result, L, ud, tname)
#define qlL_where(L, lvl) foreign "C" qlL_where(L, lvl)
#define qlL_error(result, L, fmt) foreign "C" qlL_error(result, L, fmt)
#define qlL_checkoption(result, L, arg, def, lst) foreign "C" qlL_checkoption(result, L, arg, def, lst)
#define qlL_fileresult(result, L, stat, fname) foreign "C" qlL_fileresult(result, L, stat, fname)
#define qlL_execresult(result, L, stat) foreign "C" qlL_execresult(result, L, stat)
#define qlL_ref(result, L, t) foreign "C" qlL_ref(result, L, t)
#define qlL_unref(L, t, ref) foreign "C" qlL_unref(L, t, ref)
#define qlL_loadfilex(result, L, filename, mode) foreign "C" qlL_loadfilex(result, L, filename, mode)
#define qlL_loadfile(result, L, f) foreign "C" qlL_loadfile(result, L, f)
#define qlL_loadbufferx(result, L, buff, sz, name, mode) foreign "C" qlL_loadbufferx(result, L, buff, sz, name, mode)
#define qlL_loadbuffer(result, L, s, sz, n) foreign "C" qlL_loadbuffer(result, L, s, sz, n)
#define qlL_loadstring(result, L, s) foreign "C" qlL_loadstring(result, L, s)
#define qlL_newstate(result) foreign "C" qlL_newstate(result)
#define qlL_len(result, L, idx) foreign "C" qlL_len(result, L, idx)
#define qlL_addgsub(B, s, p, r) foreign "C" qlL_addgsub(B, s, p, r)
#define qlL_gsub(result_ptr, L, s, p, r) foreign "C" qlL_gsub(result_ptr, L, s, p, r)
#define qlL_setfuncs(L, l, nup) foreign "C" qlL_setfuncs(L, l, nup)
#define qlL_getsubtable(result, L, idx, fname) foreign "C" qlL_getsubtable(result, L, idx, fname)
#define qlL_traceback(L, L1, msg, level) foreign "C" qlL_traceback(L, L1, msg, level)
#define qlL_requiref(L, modname, openf, glb) foreign "C" qlL_requiref(L, modname, openf, glb)
#define qlL_checkversion_aux(L) foreign "C" qlL_checkversion_aux(L)
#define qlL_typename(result_ptr, L, i) foreign "C" qlL_typename(result_ptr, L, i)
#define qlL_argcheck_stub(L, arg) foreign "C" qlL_argcheck_stub(L, arg)
#define qlL_checkstring(result_ptr, L, n) foreign "C" qlL_checkstring(result_ptr, L, n)
#define qlL_optstring(result_ptr, L, n, d) foreign "C" qlL_optstring(result_ptr, L, n, d)
#define qlL_dofile(result, L, fn) foreign "C" qlL_dofile(result, L, fn)
#define qlL_dostring(result, L, s) foreign "C" qlL_dostring(result, L, s)
#define qlL_getmetatable_aux(result, L, n) foreign "C" qlL_getmetatable_aux(result, L, n)
#define qlL_opt_aux(result, L, f, n, d) foreign "C" qlL_opt_aux(result, L, f, n, d)
#define qlL_pushfail(L) ql_pushnil(L)
#define qlL_newlibtable(L, l_size) foreign "C" qlL_newlibtable(L, l_size)
#define qlL_newlib(L, l_size) foreign "C" qlL_newlib(L, l_size)

/* ========================================
 * Buffer API (simplified for Cmm)
 * ======================================== */
#define qlL_buffinit(L, B) foreign "C" qlL_buffinit(L, B)
#define qlL_prepbuffsize(result, B, sz) foreign "C" qlL_prepbuffsize(result, B, sz)
#define qlL_addlstring(B, s, l) foreign "C" qlL_addlstring(B, s, l)
#define qlL_addstring(B, s) foreign "C" qlL_addstring(B, s)
#define qlL_addvalue(B) foreign "C" qlL_addvalue(B)
#define qlL_pushresult(B) foreign "C" qlL_pushresult(B)
#define qlL_pushresultsize(B, sz) foreign "C" qlL_pushresultsize(B, sz)
#define qlL_buffinitsize(result, L, B, sz) foreign "C" qlL_buffinitsize(result, L, B, sz)
#define qlL_addchar(B, c) foreign "C" qlL_addchar(B, c)
#define qlL_addsize(B, s) foreign "C" qlL_addsize(B, s)

/* ========================================
 * Standard libraries
 * ======================================== */
#define ql_openlibs(L) foreign "C" ql_openlibs(L)
#define ql_open_base(result, L) foreign "C" ql_open_base(result, L)
#define ql_open_coroutine(result, L) foreign "C" ql_open_coroutine(result, L)
#define ql_open_table(result, L) foreign "C" ql_open_table(result, L)
#define ql_open_io(result, L) foreign "C" ql_open_io(result, L)
#define ql_open_os(result, L) foreign "C" ql_open_os(result, L)
#define ql_open_string(result, L) foreign "C" ql_open_string(result, L)
#define ql_open_utf8(result, L) foreign "C" ql_open_utf8(result, L)
#define ql_open_math(result, L) foreign "C" ql_open_math(result, L)
#define ql_open_debug(result, L) foreign "C" ql_open_debug(result, L)
#define ql_open_package(result, L) foreign "C" ql_open_package(result, L)

/* ========================================
 * Extra utilities
 * ======================================== */
#define ql_register(L, n, f) (ql_pushcfunction(L, f), ql_setglobal(L, n))
#define ql_replace(L, idx) (ql_copy(L, -1, idx), ql_pop(L, 1))
#define ql_insert(L, idx) ql_rotate(L, idx, 1)
#define ql_remove(L, idx) (ql_rotate(L, idx, -1), ql_pop(L, 1))
#define qlL_dostring_simple(L, s) qlL_dostring(result_dummy, L, s)
#define qlL_dofile_simple(L, fn) qlL_dofile(result_dummy, L, fn)
