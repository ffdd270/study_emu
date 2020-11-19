//
// Created by nhy20 on 2020-11-20.
//

#ifndef GAMEBOY_LUA_UTIL_H
#define GAMEBOY_LUA_UTIL_H

#include "gameboy_luabinding.h"
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

template <typename T>
T getValue(lua_State * L, const std::string & value_name )
{
	return luabridge::getGlobal(L, value_name.c_str()).cast<T>();
}


inline void runLua(lua_State * L, const std::string& script)
{
	if (luaL_loadstring(L, script.c_str()) != 0)
	{
		throw std::runtime_error(lua_tostring(L, -1));
	}

	if (lua_pcall(L, 0, 0, -2) != 0)
	{
		throw std::runtime_error(lua_tostring(L, -1));
	}
}


#endif //GAMEBOY_LUA_UTIL_H
