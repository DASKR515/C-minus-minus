#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

int run_lua_logic() {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    luaL_dofile(L, "script.lua");

    lua_close(L);
    return 0;
}