#pragma once
#include <cstdint>
#include "lua_natives_rvas.h"
#include "Engine/Game/Build/fingerprint.h"

// The game's Lua is stock Lua 5.2 with every standard library open. These are the functions in
// it that reach outside Lua (Extension/Scripting/lua_native_block.h). Found from the libraries'
// registration tables in .rodata (package at 0x6445c30, os at 0x6445f90, io at 0x6446150).
namespace dingosdk::game::build::v20260929::lua_natives {
// int ll_loadfunc(lua_State*, const char* path, const char* symbol): the one place Lua loads a
// DLL (LoadLibraryExA). package.loadlib calls it, and so does require through both C searchers
// (by way of loadfunc at 0x334cfa0). 0 on success; 1 with a message pushed when the DLL does
// not load.

// const char* lua_pushfstring(lua_State*, const char* format, ...).

// void f_parser(lua_State*, SParser*): compiles a chunk, or undumps one that starts with the
// bytecode signature. It checks the chunk's kind against SParser::mode.

// const char* SParser::mode ("b", "t", "bt" or null for both); name follows at 0x00.

// lua_CFunctions of the os and io libraries.






}
