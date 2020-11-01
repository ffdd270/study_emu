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
	LuaContext();

	bool ExecuteFunction( std::string_view func_name );
	bool ExecuteString( std::string_view execute_string );

	bool IsExistGlobalValue( std::string_view value_name );

	void Reload();

	std::string_view GetLastError();

	~LuaContext();
private:
	void init();
private:
	lua_State * mLuaState = nullptr;
};


#endif //GAMEBOY_LUACONTEXT_H
