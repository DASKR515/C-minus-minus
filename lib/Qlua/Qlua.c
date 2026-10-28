/*
              --    by JABIR aka <DASKR>
              --    Qlua.c - C implementation for Qlua bindings
              --    Lua 5.4.6 integration layer
              --    ver <1.0.0>
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "src/lua-5.4.6/src/lua.h"
#include "src/lua-5.4.6/src/lauxlib.h"
#include "src/lua-5.4.6/src/lualib.h"



static void ql_memcpy_to_ptr(void *dst, const void *src, size_t sz) {
    if (dst != NULL && src != NULL) {
        memcpy(dst, src, sz);
    }
}

static void ql_memcpy_from_ptr(void *dst, const void *src, size_t sz) {
    if (dst != NULL && src != NULL) {
        memcpy(dst, src, sz);
    }
}

void ql_newstate(void *result) {
    lua_State *L = luaL_newstate();
    memcpy(result, &L, sizeof(L));
}

void ql_close(lua_State *L) {
    if (L != NULL) {
        lua_close(L);
    }
}

void ql_newthread(void *result, lua_State *L) {
    lua_State *NL = NULL;
    if (L != NULL) {
        NL = lua_newthread(L);
    }
    memcpy(result, &NL, sizeof(NL));
}

void ql_closethread(void *result, lua_State *L, lua_State *from) {
    int r = -1;
    if (L != NULL) {
        r = lua_closethread(L, from);
    }
    memcpy(result, &r, sizeof(r));
}

void *ql_atpanic(lua_State *L, lua_CFunction panicf) {
    if (L == NULL) {
        return NULL;
    }
    return (void *)(intptr_t)lua_atpanic(L, panicf);
}

void ql_version(void *result, lua_State *L) {
    lua_Number v = 0;
    if (L != NULL) {
        v = lua_version(L);
    } else {
        v = LUA_VERSION_NUM;
    }
    memcpy(result, &v, sizeof(v));
}

void ql_status(void *result, lua_State *L) {
    int s = -1;
    if (L != NULL) {
        s = lua_status(L);
    }
    memcpy(result, &s, sizeof(s));
}

void ql_isyieldable_res(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = lua_isyieldable(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_absindex(void *result, lua_State *L, int idx) {
    int r = 0;
    if (L != NULL) {
        r = lua_absindex(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_gettop(void *result, lua_State *L) {
    int r = -1;
    if (L != NULL) {
        r = lua_gettop(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_settop(lua_State *L, int idx) {
    if (L != NULL) {
        lua_settop(L, idx);
    }
}

void ql_pushvalue(lua_State *L, int idx) {
    if (L != NULL) {
        lua_pushvalue(L, idx);
    }
}

void ql_rotate(lua_State *L, int idx, int n) {
    if (L != NULL) {
        lua_rotate(L, idx, n);
    }
}

void ql_copy(lua_State *L, int fromidx, int toidx) {
    if (L != NULL) {
        lua_copy(L, fromidx, toidx);
    }
}

void ql_checkstack(void *result, lua_State *L, int n) {
    int r = 0;
    if (L != NULL) {
        r = lua_checkstack(L, n);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_xmove(lua_State *from, lua_State *to, int n) {
    if (from != NULL && to != NULL) {
        lua_xmove(from, to, n);
    }
}

void ql_isnumber(void *result, lua_State *L, int idx) {
    int r = 0;
    if (L != NULL) {
        r = lua_isnumber(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_isstring(void *result, lua_State *L, int idx) {
    int r = 0;
    if (L != NULL) {
        r = lua_isstring(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_iscfunction(void *result, lua_State *L, int idx) {
    int r = 0;
    if (L != NULL) {
        r = lua_iscfunction(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_isinteger(void *result, lua_State *L, int idx) {
    int r = 0;
    if (L != NULL) {
        r = lua_isinteger(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_isuserdata(void *result, lua_State *L, int idx) {
    int r = 0;
    if (L != NULL) {
        r = lua_isuserdata(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_type(void *result, lua_State *L, int idx) {
    int r = LUA_TNONE;
    if (L != NULL) {
        r = lua_type(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_typename(void *result_ptr, lua_State *L, int tp) {
    const char *s = "";
    if (L != NULL) {
        s = lua_typename(L, tp);
    }
    memcpy(result_ptr, &s, sizeof(s));
}

void ql_tonumberx(void *result, lua_State *L, int idx, int *isnum) {
    lua_Number v = 0;
    if (L != NULL) {
        v = lua_tonumberx(L, idx, isnum);
    }
    memcpy(result, &v, sizeof(v));
}

void ql_tointegerx(void *result, lua_State *L, int idx, int *isnum) {
    lua_Integer v = 0;
    if (L != NULL) {
        v = lua_tointegerx(L, idx, isnum);
    }
    memcpy(result, &v, sizeof(v));
}

void ql_tonumber(void *result, lua_State *L, int idx) {
    lua_Number v = 0;
    if (L != NULL) {
        v = lua_tonumber(L, idx);
    }
    memcpy(result, &v, sizeof(v));
}

void ql_tointeger(void *result, lua_State *L, int idx) {
    lua_Integer v = 0;
    if (L != NULL) {
        v = lua_tointeger(L, idx);
    }
    memcpy(result, &v, sizeof(v));
}

void ql_toboolean(void *result, lua_State *L, int idx) {
    int r = 0;
    if (L != NULL) {
        r = lua_toboolean(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_tolstring(void *result_ptr, lua_State *L, int idx, size_t *len) {
    const char *s = NULL;
    if (L != NULL) {
        s = lua_tolstring(L, idx, len);
    }
    memcpy(result_ptr, &s, sizeof(s));
}

void ql_rawlen(void *result, lua_State *L, int idx) {
    lua_Unsigned r = 0;
    if (L != NULL) {
        r = lua_rawlen(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_tocfunction(void *result, lua_State *L, int idx) {
    lua_CFunction f = NULL;
    if (L != NULL) {
        f = lua_tocfunction(L, idx);
    }
    memcpy(result, &f, sizeof(f));
}

void ql_touserdata(void *result, lua_State *L, int idx) {
    void *p = NULL;
    if (L != NULL) {
        p = lua_touserdata(L, idx);
    }
    memcpy(result, &p, sizeof(p));
}

void ql_tothread(void *result, lua_State *L, int idx) {
    lua_State *t = NULL;
    if (L != NULL) {
        t = lua_tothread(L, idx);
    }
    memcpy(result, &t, sizeof(t));
}

void ql_topointer(void *result, lua_State *L, int idx) {
    const void *p = NULL;
    if (L != NULL) {
        p = lua_topointer(L, idx);
    }
    memcpy(result, &p, sizeof(p));
}

void ql_tostring(void *result_ptr, lua_State *L, int idx) {
    const char *s = NULL;
    if (L != NULL) {
        s = lua_tostring(L, idx);
    }
    memcpy(result_ptr, &s, sizeof(s));
}

void ql_isfunction(void *result, lua_State *L, int n) {
    int r = 0;
    if (L != NULL) r = lua_isfunction(L, n);
    memcpy(result, &r, sizeof(r));
}

void ql_istable(void *result, lua_State *L, int n) {
    int r = 0;
    if (L != NULL) r = lua_istable(L, n);
    memcpy(result, &r, sizeof(r));
}

void ql_isnil(void *result, lua_State *L, int n) {
    int r = 0;
    if (L != NULL) r = lua_isnil(L, n);
    memcpy(result, &r, sizeof(r));
}

void ql_isboolean(void *result, lua_State *L, int n) {
    int r = 0;
    if (L != NULL) r = lua_isboolean(L, n);
    memcpy(result, &r, sizeof(r));
}

void ql_isthread(void *result, lua_State *L, int n) {
    int r = 0;
    if (L != NULL) r = lua_isthread(L, n);
    memcpy(result, &r, sizeof(r));
}

void ql_isnone(void *result, lua_State *L, int n) {
    int r = 0;
    if (L != NULL) r = lua_isnone(L, n);
    memcpy(result, &r, sizeof(r));
}

void ql_isnoneornil(void *result, lua_State *L, int n) {
    int r = 0;
    if (L != NULL) r = lua_isnoneornil(L, n);
    memcpy(result, &r, sizeof(r));
}

void ql_islightuserdata(void *result, lua_State *L, int n) {
    int r = 0;
    if (L != NULL) r = lua_islightuserdata(L, n);
    memcpy(result, &r, sizeof(r));
}

void ql_pushnil(lua_State *L) {
    if (L != NULL) lua_pushnil(L);
}

void ql_pushnumber(lua_State *L, lua_Number n) {
    if (L != NULL) lua_pushnumber(L, n);
}

void ql_pushinteger(lua_State *L, lua_Integer n) {
    if (L != NULL) lua_pushinteger(L, n);
}

void ql_pushlstring(void *result_ptr, lua_State *L, const char *s, size_t len) {
    const char *r = NULL;
    if (L != NULL && s != NULL) {
        r = lua_pushlstring(L, s, len);
    } else if (L != NULL) {
        r = lua_pushlstring(L, "", 0);
    }
    memcpy(result_ptr, &r, sizeof(r));
}

void ql_pushstring(void *result_ptr, lua_State *L, const char *s) {
    const char *r = NULL;
    if (L != NULL) {
        r = lua_pushstring(L, s);
    }
    memcpy(result_ptr, &r, sizeof(r));
}

void ql_pushfstring(void *result_ptr, lua_State *L, const char *fmt, ...) {
    const char *r = NULL;
    if (L != NULL && fmt != NULL) {
        va_list argp;
        va_start(argp, fmt);
        r = lua_pushvfstring(L, fmt, argp);
        va_end(argp);
    }
    memcpy(result_ptr, &r, sizeof(r));
}

void ql_pushcclosure(lua_State *L, lua_CFunction fn, int n) {
    if (L != NULL && fn != NULL) {
        lua_pushcclosure(L, fn, n);
    } else if (L != NULL) {
        lua_pushnil(L);
    }
}

void ql_pushboolean(lua_State *L, int b) {
    if (L != NULL) lua_pushboolean(L, b);
}

void ql_pushlightuserdata(lua_State *L, void *p) {
    if (L != NULL) lua_pushlightuserdata(L, p);
}

void ql_pushthread(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = lua_pushthread(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_pushglobaltable(lua_State *L) {
    if (L != NULL) lua_pushglobaltable(L);
}


void ql_getglobal(void *result, lua_State *L, const char *name) {
    int r = LUA_TNIL;
    if (L != NULL && name != NULL) {
        r = lua_getglobal(L, name);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_setglobal(lua_State *L, const char *name) {
    if (L != NULL && name != NULL) {
        lua_setglobal(L, name);
    }
}

void ql_gettable(void *result, lua_State *L, int idx) {
    int r = LUA_TNIL;
    if (L != NULL) {
        r = lua_gettable(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_settable(lua_State *L, int idx) {
    if (L != NULL) {
        lua_settable(L, idx);
    }
}

void ql_getfield(void *result, lua_State *L, int idx, const char *k) {
    int r = LUA_TNIL;
    if (L != NULL && k != NULL) {
        r = lua_getfield(L, idx, k);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_setfield(lua_State *L, int idx, const char *k) {
    if (L != NULL && k != NULL) {
        lua_setfield(L, idx, k);
    }
}

void ql_geti(void *result, lua_State *L, int idx, lua_Integer n) {
    int r = LUA_TNIL;
    if (L != NULL) {
        r = lua_geti(L, idx, n);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_seti(lua_State *L, int idx, lua_Integer n) {
    if (L != NULL) {
        lua_seti(L, idx, n);
    }
}

void ql_rawget(void *result, lua_State *L, int idx) {
    int r = LUA_TNIL;
    if (L != NULL) {
        r = lua_rawget(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_rawset(lua_State *L, int idx) {
    if (L != NULL) {
        lua_rawset(L, idx);
    }
}

void ql_rawgeti(void *result, lua_State *L, int idx, lua_Integer n) {
    int r = LUA_TNIL;
    if (L != NULL) {
        r = lua_rawgeti(L, idx, n);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_rawseti(lua_State *L, int idx, lua_Integer n) {
    if (L != NULL) {
        lua_rawseti(L, idx, n);
    }
}

void ql_rawgetp(void *result, lua_State *L, int idx, const void *p) {
    int r = LUA_TNIL;
    if (L != NULL && p != NULL) {
        r = lua_rawgetp(L, idx, p);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_rawsetp(lua_State *L, int idx, const void *p) {
    if (L != NULL && p != NULL) {
        lua_rawsetp(L, idx, p);
    }
}

void ql_createtable(lua_State *L, int narr, int nrec) {
    if (L != NULL) {
        lua_createtable(L, narr, nrec);
    }
}

void ql_newuserdatauv(void *result, lua_State *L, size_t sz, int nuvalue) {
    void *p = NULL;
    if (L != NULL) {
        p = lua_newuserdatauv(L, sz, nuvalue);
    }
    memcpy(result, &p, sizeof(p));
}

void ql_newuserdata(void *result, lua_State *L, size_t sz) {
    void *p = NULL;
    if (L != NULL) {
        p = lua_newuserdatauv(L, sz, 1);
    }
    memcpy(result, &p, sizeof(p));
}

void ql_getmetatable(void *result, lua_State *L, int objindex) {
    int r = 0;
    if (L != NULL) {
        r = lua_getmetatable(L, objindex);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_setmetatable(void *result, lua_State *L, int objindex) {
    int r = 0;
    if (L != NULL) {
        r = lua_setmetatable(L, objindex);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_getiuservalue(void *result, lua_State *L, int idx, int n) {
    int r = LUA_TNIL;
    if (L != NULL) {
        r = lua_getiuservalue(L, idx, n);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_setiuservalue(void *result, lua_State *L, int idx, int n) {
    int r = 0;
    if (L != NULL) {
        r = lua_setiuservalue(L, idx, n);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_getuservalue(void *result, lua_State *L, int idx) {
    int r = LUA_TNIL;
    if (L != NULL) {
        r = lua_getiuservalue(L, idx, 1);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_setuservalue(void *result, lua_State *L, int idx) {
    int r = 0;
    if (L != NULL) {
        r = lua_setiuservalue(L, idx, 1);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_arith(lua_State *L, int op) {
    if (L != NULL) {
        lua_arith(L, op);
    }
}

void ql_rawequal(void *result, lua_State *L, int idx1, int idx2) {
    int r = 0;
    if (L != NULL) {
        r = lua_rawequal(L, idx1, idx2);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_compare(void *result, lua_State *L, int idx1, int idx2, int op) {
    int r = 0;
    if (L != NULL) {
        r = lua_compare(L, idx1, idx2, op);
    }
    memcpy(result, &r, sizeof(r));
}


void ql_call(lua_State *L, int nargs, int nresults) {
    if (L != NULL) {
        lua_call(L, nargs, nresults);
    }
}

void ql_callk(lua_State *L, int nargs, int nresults, lua_KContext ctx, lua_KFunction k) {
    if (L != NULL) {
        lua_callk(L, nargs, nresults, ctx, k);
    }
}

void ql_pcall(void *result, lua_State *L, int nargs, int nresults, int errfunc) {
    int r = LUA_ERRERR;
    if (L != NULL) {
        r = lua_pcall(L, nargs, nresults, errfunc);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_pcallk(void *result, lua_State *L, int nargs, int nresults, int errfunc, lua_KContext ctx, lua_KFunction k) {
    int r = LUA_ERRERR;
    if (L != NULL) {
        r = lua_pcallk(L, nargs, nresults, errfunc, ctx, k);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_load(void *result, lua_State *L, lua_Reader reader, void *dt, const char *chunkname, const char *mode) {
    int r = LUA_ERRERR;
    if (L != NULL) {
        r = lua_load(L, reader, dt, chunkname, mode);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_dump(void *result, lua_State *L, lua_Writer writer, void *data, int strip) {
    int r = 0;
    if (L != NULL) {
        r = lua_dump(L, writer, data, strip);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_yield(void *result, lua_State *L, int nresults) {
    int r = 0;
    if (L != NULL) {
        r = lua_yield(L, nresults);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_yieldk(void *result, lua_State *L, int nresults, lua_KContext ctx, lua_KFunction k) {
    int r = 0;
    if (L != NULL) {
        r = lua_yieldk(L, nresults, ctx, k);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_resume(void *result, lua_State *L, lua_State *from, int narg, int *nres) {
    int r = LUA_ERRERR;
    if (L != NULL) {
        r = lua_resume(L, from, narg, nres);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_isyieldable(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = lua_isyieldable(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_gc(void *result, lua_State *L, int what) {
    int r = -1;
    if (L != NULL) {
        r = lua_gc(L, what);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_gc_int(void *result, lua_State *L, int what, int arg) {
    int r = -1;
    if (L != NULL) {
        r = lua_gc(L, what, arg);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_error(void *result, lua_State *L) {
    int r = -1;
    if (L != NULL) {
        r = lua_error(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_next(void *result, lua_State *L, int idx) {
    int r = 0;
    if (L != NULL) {
        r = lua_next(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_concat(lua_State *L, int n) {
    if (L != NULL) {
        lua_concat(L, n);
    }
}

void ql_len(lua_State *L, int idx) {
    if (L != NULL) {
        lua_len(L, idx);
    }
}

void ql_stringtonumber(void *result, lua_State *L, const char *s) {
    size_t r = 0;
    if (L != NULL && s != NULL) {
        r = lua_stringtonumber(L, s);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_getallocf(void *result, lua_State *L, void **ud) {
    lua_Alloc f = NULL;
    if (L != NULL) {
        f = lua_getallocf(L, ud);
    }
    memcpy(result, &f, sizeof(f));
}

void ql_setallocf(lua_State *L, lua_Alloc f, void *ud) {
    if (L != NULL) {
        lua_setallocf(L, f, ud);
    }
}

void ql_toclose(lua_State *L, int idx) {
    if (L != NULL) {
        lua_toclose(L, idx);
    }
}

void ql_closeslot(lua_State *L, int idx) {
    if (L != NULL) {
        lua_closeslot(L, idx);
    }
}

void ql_setwarnf(lua_State *L, lua_WarnFunction f, void *ud) {
    if (L != NULL) {
        lua_setwarnf(L, f, ud);
    }
}

void ql_warning(lua_State *L, const char *msg, int tocont) {
    if (L != NULL && msg != NULL) {
        lua_warning(L, msg, tocont);
    }
}

void ql_getstack(void *result, lua_State *L, int level, lua_Debug *ar) {
    int r = 0;
    if (L != NULL && ar != NULL) {
        r = lua_getstack(L, level, ar);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_getinfo(void *result, lua_State *L, const char *what, lua_Debug *ar) {
    int r = 0;
    if (L != NULL && what != NULL && ar != NULL) {
        r = lua_getinfo(L, what, ar);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_getlocal(void *result_ptr, lua_State *L, const lua_Debug *ar, int n) {
    const char *r = NULL;
    if (L != NULL && ar != NULL) {
        r = lua_getlocal(L, ar, n);
    }
    memcpy(result_ptr, &r, sizeof(r));
}

void ql_setlocal(void *result_ptr, lua_State *L, const lua_Debug *ar, int n) {
    const char *r = NULL;
    if (L != NULL && ar != NULL) {
        r = lua_setlocal(L, ar, n);
    }
    memcpy(result_ptr, &r, sizeof(r));
}

void ql_getupvalue(void *result_ptr, lua_State *L, int funcindex, int n) {
    const char *r = NULL;
    if (L != NULL) {
        r = lua_getupvalue(L, funcindex, n);
    }
    memcpy(result_ptr, &r, sizeof(r));
}

void ql_setupvalue(void *result_ptr, lua_State *L, int funcindex, int n) {
    const char *r = NULL;
    if (L != NULL) {
        r = lua_setupvalue(L, funcindex, n);
    }
    memcpy(result_ptr, &r, sizeof(r));
}

void ql_upvalueid(void *result, lua_State *L, int fidx, int n) {
    void *p = NULL;
    if (L != NULL) {
        p = lua_upvalueid(L, fidx, n);
    }
    memcpy(result, &p, sizeof(p));
}

void ql_upvaluejoin(lua_State *L, int fidx1, int n1, int fidx2, int n2) {
    if (L != NULL) {
        lua_upvaluejoin(L, fidx1, n1, fidx2, n2);
    }
}

void ql_sethook(lua_State *L, lua_Hook func, int mask, int count) {
    if (L != NULL) {
        lua_sethook(L, func, mask, count);
    }
}

void ql_gethook(void *result, lua_State *L) {
    lua_Hook f = NULL;
    if (L != NULL) {
        f = lua_gethook(L);
    }
    memcpy(result, &f, sizeof(f));
}

void ql_gethookmask(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = lua_gethookmask(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_gethookcount(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = lua_gethookcount(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_setcstacklimit(void *result, lua_State *L, unsigned int limit) {
    int r = 0;
    if (L != NULL) {
        r = lua_setcstacklimit(L, limit);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_checkversion(lua_State *L) {
    if (L != NULL) {
        luaL_checkversion(L);
    }
}

void qlL_checkversion_aux(lua_State *L) {
    if (L != NULL) {
        luaL_checkversion(L);
    }
}

void qlL_getmetafield(void *result, lua_State *L, int obj, const char *e) {
    int r = 0;
    if (L != NULL && e != NULL) {
        r = luaL_getmetafield(L, obj, e);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_callmeta(void *result, lua_State *L, int obj, const char *e) {
    int r = 0;
    if (L != NULL && e != NULL) {
        r = luaL_callmeta(L, obj, e);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_tolstring(void *result_ptr, lua_State *L, int idx, size_t *len) {
    const char *s = NULL;
    if (L != NULL) {
        s = luaL_tolstring(L, idx, len);
    }
    memcpy(result_ptr, &s, sizeof(s));
}

void qlL_argerror(void *result, lua_State *L, int arg, const char *extramsg) {
    int r = -1;
    if (L != NULL) {
        r = luaL_argerror(L, arg, extramsg);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_typeerror(void *result, lua_State *L, int arg, const char *tname) {
    int r = -1;
    if (L != NULL) {
        r = luaL_typeerror(L, arg, tname);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_checklstring(void *result_ptr, lua_State *L, int arg, size_t *l) {
    const char *s = NULL;
    if (L != NULL) {
        s = luaL_checklstring(L, arg, l);
    }
    memcpy(result_ptr, &s, sizeof(s));
}

void qlL_optlstring(void *result_ptr, lua_State *L, int arg, const char *def, size_t *l) {
    const char *s = NULL;
    if (L != NULL) {
        s = luaL_optlstring(L, arg, def, l);
    }
    memcpy(result_ptr, &s, sizeof(s));
}

void qlL_checknumber(void *result, lua_State *L, int arg) {
    lua_Number v = 0;
    if (L != NULL) {
        v = luaL_checknumber(L, arg);
    }
    memcpy(result, &v, sizeof(v));
}

void qlL_optnumber(void *result, lua_State *L, int arg, lua_Number def) {
    lua_Number v = def;
    if (L != NULL) {
        v = luaL_optnumber(L, arg, def);
    }
    memcpy(result, &v, sizeof(v));
}

void qlL_checkinteger(void *result, lua_State *L, int arg) {
    lua_Integer v = 0;
    if (L != NULL) {
        v = luaL_checkinteger(L, arg);
    }
    memcpy(result, &v, sizeof(v));
}

void qlL_optinteger(void *result, lua_State *L, int arg, lua_Integer def) {
    lua_Integer v = def;
    if (L != NULL) {
        v = luaL_optinteger(L, arg, def);
    }
    memcpy(result, &v, sizeof(v));
}

void qlL_checkstack_aux(lua_State *L, int sz, const char *msg) {
    if (L != NULL) {
        luaL_checkstack(L, sz, msg);
    }
}

void qlL_checktype(lua_State *L, int arg, int t) {
    if (L != NULL) {
        luaL_checktype(L, arg, t);
    }
}

void qlL_checkany(lua_State *L, int arg) {
    if (L != NULL) {
        luaL_checkany(L, arg);
    }
}

void qlL_newmetatable(void *result, lua_State *L, const char *tname) {
    int r = 0;
    if (L != NULL && tname != NULL) {
        r = luaL_newmetatable(L, tname);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_setmetatable(lua_State *L, const char *tname) {
    if (L != NULL && tname != NULL) {
        luaL_setmetatable(L, tname);
    }
}

void qlL_testudata(void *result, lua_State *L, int ud, const char *tname) {
    void *p = NULL;
    if (L != NULL && tname != NULL) {
        p = luaL_testudata(L, ud, tname);
    }
    memcpy(result, &p, sizeof(p));
}

void qlL_checkudata(void *result, lua_State *L, int ud, const char *tname) {
    void *p = NULL;
    if (L != NULL && tname != NULL) {
        p = luaL_checkudata(L, ud, tname);
    }
    memcpy(result, &p, sizeof(p));
}

void qlL_where(lua_State *L, int lvl) {
    if (L != NULL) {
        luaL_where(L, lvl);
    }
}

void qlL_error(void *result, lua_State *L, const char *fmt) {
    int r = -1;
    if (L != NULL && fmt != NULL) {
        r = luaL_error(L, fmt);
    } else if (L != NULL) {
        r = luaL_error(L, "");
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_checkoption(void *result, lua_State *L, int arg, const char *def, const char *const lst[]) {
    int r = -1;
    if (L != NULL && lst != NULL) {
        r = luaL_checkoption(L, arg, def, lst);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_fileresult(void *result, lua_State *L, int stat, const char *fname) {
    int r = 0;
    if (L != NULL) {
        r = luaL_fileresult(L, stat, fname);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_execresult(void *result, lua_State *L, int stat) {
    int r = 0;
    if (L != NULL) {
        r = luaL_execresult(L, stat);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_ref(void *result, lua_State *L, int t) {
    int r = LUA_NOREF;
    if (L != NULL) {
        r = luaL_ref(L, t);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_unref(lua_State *L, int t, int ref) {
    if (L != NULL) {
        luaL_unref(L, t, ref);
    }
}

void qlL_loadfilex(void *result, lua_State *L, const char *filename, const char *mode) {
    int r = LUA_ERRFILE;
    if (L != NULL) {
        r = luaL_loadfilex(L, filename, mode);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_loadfile(void *result, lua_State *L, const char *f) {
    int r = LUA_ERRFILE;
    if (L != NULL) {
        r = luaL_loadfile(L, f);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_loadbufferx(void *result, lua_State *L, const char *buff, size_t sz, const char *name, const char *mode) {
    int r = LUA_ERRERR;
    if (L != NULL && buff != NULL) {
        r = luaL_loadbufferx(L, buff, sz, name, mode);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_loadbuffer(void *result, lua_State *L, const char *s, size_t sz, const char *n) {
    int r = LUA_ERRERR;
    if (L != NULL && s != NULL) {
        r = luaL_loadbuffer(L, s, sz, n);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_loadstring(void *result, lua_State *L, const char *s) {
    int r = LUA_ERRERR;
    if (L != NULL && s != NULL) {
        r = luaL_loadstring(L, s);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_newstate(void *result) {
    lua_State *L = luaL_newstate();
    memcpy(result, &L, sizeof(L));
}

void qlL_len(void *result, lua_State *L, int idx) {
    lua_Integer r = 0;
    if (L != NULL) {
        r = luaL_len(L, idx);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_addgsub(luaL_Buffer *B, const char *s, const char *p, const char *rstr) {
    if (B != NULL && s != NULL && p != NULL && rstr != NULL) {
        luaL_addgsub(B, s, p, rstr);
    }
}

void qlL_gsub(void *result_ptr, lua_State *L, const char *s, const char *p, const char *rstr) {
    const char *r = NULL;
    if (L != NULL && s != NULL && p != NULL && rstr != NULL) {
        r = luaL_gsub(L, s, p, rstr);
    }
    memcpy(result_ptr, &r, sizeof(r));
}

void qlL_setfuncs(lua_State *L, const luaL_Reg *l, int nup) {
    if (L != NULL && l != NULL) {
        luaL_setfuncs(L, l, nup);
    }
}

void qlL_getsubtable(void *result, lua_State *L, int idx, const char *fname) {
    int r = 0;
    if (L != NULL && fname != NULL) {
        r = luaL_getsubtable(L, idx, fname);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_traceback(lua_State *L, lua_State *L1, const char *msg, int level) {
    if (L != NULL) {
        luaL_traceback(L, L1, msg, level);
    }
}

void qlL_requiref(lua_State *L, const char *modname, lua_CFunction openf, int glb) {
    if (L != NULL && modname != NULL && openf != NULL) {
        luaL_requiref(L, modname, openf, glb);
    }
}

void qlL_typename(void *result_ptr, lua_State *L, int i) {
    const char *r = "";
    if (L != NULL) {
        r = luaL_typename(L, i);
    }
    memcpy(result_ptr, &r, sizeof(r));
}

void qlL_checkstring(void *result_ptr, lua_State *L, int n) {
    const char *r = NULL;
    if (L != NULL) {
        r = luaL_checkstring(L, n);
    }
    memcpy(result_ptr, &r, sizeof(r));
}

void qlL_optstring(void *result_ptr, lua_State *L, int n, const char *d) {
    const char *r = d;
    if (L != NULL) {
        r = luaL_optstring(L, n, d);
    }
    memcpy(result_ptr, &r, sizeof(r));
}

void qlL_dofile(void *result, lua_State *L, const char *fn) {
    int r = -1;
    if (L != NULL) {
        r = luaL_dofile(L, fn);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_dostring(void *result, lua_State *L, const char *s) {
    int r = -1;
    if (L != NULL) {
        r = luaL_dostring(L, s);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_getmetatable_aux(void *result, lua_State *L, const char *n) {
    int r = LUA_TNIL;
    if (L != NULL && n != NULL) {
        r = lua_getfield(L, LUA_REGISTRYINDEX, n);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_opt_aux(void *result, lua_State *L, lua_Number (*f)(lua_State *, int), int n, lua_Number d) {
    lua_Number r = d;
    if (L != NULL && f != NULL && lua_isnoneornil(L, n)) {
        r = d;
    } else if (L != NULL && f != NULL) {
        r = f(L, n);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_newlibtable(lua_State *L, size_t l_size) {
    if (L != NULL) {
        /* Approximate - Cmm passes size as count; safer to create empty table */
        lua_createtable(L, 0, (int)(l_size > 0 ? l_size - 1 : 0));
    }
}

void qlL_newlib(lua_State *L, size_t l_size) {
    if (L != NULL) {
        luaL_checkversion(L);
        lua_createtable(L, 0, (int)(l_size > 0 ? l_size - 1 : 0));
    }
}

void qlL_buffinit(lua_State *L, luaL_Buffer *B) {
    if (L != NULL && B != NULL) {
        luaL_buffinit(L, B);
    }
}

void qlL_prepbuffsize(void *result, luaL_Buffer *B, size_t sz) {
    char *r = NULL;
    if (B != NULL) {
        r = luaL_prepbuffsize(B, sz);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_addlstring(luaL_Buffer *B, const char *s, size_t l) {
    if (B != NULL && s != NULL) {
        luaL_addlstring(B, s, l);
    }
}

void qlL_addstring(luaL_Buffer *B, const char *s) {
    if (B != NULL && s != NULL) {
        luaL_addstring(B, s);
    }
}

void qlL_addvalue(luaL_Buffer *B) {
    if (B != NULL) {
        luaL_addvalue(B);
    }
}

void qlL_pushresult(luaL_Buffer *B) {
    if (B != NULL) {
        luaL_pushresult(B);
    }
}

void qlL_pushresultsize(luaL_Buffer *B, size_t sz) {
    if (B != NULL) {
        luaL_pushresultsize(B, sz);
    }
}

void qlL_buffinitsize(void *result, lua_State *L, luaL_Buffer *B, size_t sz) {
    char *r = NULL;
    if (L != NULL && B != NULL) {
        r = luaL_buffinitsize(L, B, sz);
    }
    memcpy(result, &r, sizeof(r));
}

void qlL_addchar(luaL_Buffer *B, char c) {
    if (B != NULL) {
        luaL_addchar(B, c);
    }
}

void qlL_addsize(luaL_Buffer *B, size_t s) {
    if (B != NULL) {
        luaL_addsize(B, s);
    }
}


void ql_openlibs(lua_State *L) {
    if (L != NULL) {
        luaL_openlibs(L);
    }
}

void ql_open_base(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = luaopen_base(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_open_coroutine(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = luaopen_coroutine(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_open_table(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = luaopen_table(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_open_io(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = luaopen_io(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_open_os(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = luaopen_os(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_open_string(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = luaopen_string(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_open_utf8(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = luaopen_utf8(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_open_math(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = luaopen_math(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_open_debug(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = luaopen_debug(L);
    }
    memcpy(result, &r, sizeof(r));
}

void ql_open_package(void *result, lua_State *L) {
    int r = 0;
    if (L != NULL) {
        r = luaopen_package(L);
    }
    memcpy(result, &r, sizeof(r));
}
