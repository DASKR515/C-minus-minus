cmake_minimum_required(VERSION 3.16)

project(Qlua VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(LUA_SOURCE_DIR
"${CMAKE_CURRENT_SOURCE_DIR}/src/lua-5.4.6/src"
)

if(NOT EXISTS "${LUA_SOURCE_DIR}/lua.h")
message(FATAL_ERROR "Lua source not found: ${LUA_SOURCE_DIR}")
endif()

set(LUA_SOURCES
${LUA_SOURCE_DIR}/lapi.c
${LUA_SOURCE_DIR}/lauxlib.c
${LUA_SOURCE_DIR}/lbaselib.c
${LUA_SOURCE_DIR}/lcode.c
${LUA_SOURCE_DIR}/lcorolib.c
${LUA_SOURCE_DIR}/lctype.c
${LUA_SOURCE_DIR}/ldblib.c
${LUA_SOURCE_DIR}/ldebug.c
${LUA_SOURCE_DIR}/ldo.c
${LUA_SOURCE_DIR}/ldump.c
${LUA_SOURCE_DIR}/lfunc.c
${LUA_SOURCE_DIR}/lgc.c
${LUA_SOURCE_DIR}/linit.c
${LUA_SOURCE_DIR}/liolib.c
${LUA_SOURCE_DIR}/llex.c
${LUA_SOURCE_DIR}/lmathlib.c
${LUA_SOURCE_DIR}/lmem.c
${LUA_SOURCE_DIR}/loadlib.c
${LUA_SOURCE_DIR}/lobject.c
${LUA_SOURCE_DIR}/lopcodes.c
${LUA_SOURCE_DIR}/loslib.c
${LUA_SOURCE_DIR}/lparser.c
${LUA_SOURCE_DIR}/lstate.c
${LUA_SOURCE_DIR}/lstring.c
${LUA_SOURCE_DIR}/lstrlib.c
${LUA_SOURCE_DIR}/ltable.c
${LUA_SOURCE_DIR}/ltablib.c
${LUA_SOURCE_DIR}/ltm.c
${LUA_SOURCE_DIR}/lundump.c
${LUA_SOURCE_DIR}/lutf8lib.c
${LUA_SOURCE_DIR}/lvm.c
${LUA_SOURCE_DIR}/lzio.c
)

add_library(Qlua SHARED
Qlua.c
${LUA_SOURCES}
)

target_include_directories(Qlua
PUBLIC
"${CMAKE_CURRENT_SOURCE_DIR}"
PRIVATE
"${LUA_SOURCE_DIR}"
)

target_compile_definitions(Qlua PRIVATE
LUA_USE_LINUX
)

target_link_libraries(Qlua PRIVATE
${CMAKE_DL_LIBS}
m
)

set_target_properties(Qlua PROPERTIES
OUTPUT_NAME Qlua
VERSION 1.0.0
SOVERSION 1
)

install(TARGETS Qlua
LIBRARY DESTINATION lib
RUNTIME DESTINATION bin
ARCHIVE DESTINATION lib
)

