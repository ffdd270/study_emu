//
// Created by nhy20 on 2020-10-29.
//

#ifndef GAMEBOY_LUACONTEXT_H
#define GAMEBOY_LUACONTEXT_H

#include <lua.hpp>
#include <string>
#include <string_view>
#include <memory>

class LuaContext
{
public:
	// Problem, Return True.
	bool ExecuteFunction( std::string_view func_name );
	std::string_view GetLastError();

	LuaContext();

	~LuaContext();
private:

	void init();
private:
	lua_State * mLuaState = nullptr;
};


#endif //GAMEBOY_LUACONTEXT_H
