//
// Created by nhy20 on 2020-10-29.
//

#include "LuaContext.h"
#include "gameboy_luabinding.h"


LuaContext::LuaContext()
{
	init();
}

bool LuaContext::ExecuteFunction(std::string_view func_name)
{
	lua_getglobal( mLuaState, func_name.data() );

	if( !lua_isfunction( mLuaState, -1 ) )
	{
		return true;
	}

	return lua_pcall( mLuaState, 0, 0, 0 );
}

LuaContext::~LuaContext()
{
	lua_close( mLuaState );
}

void LuaContext::init()
{
	mLuaState = luaL_newstate();
	luaL_openlibs( mLuaState );

	gameboy_lua_binding( mLuaState );

	luaL_dofile( mLuaState, "script/basic_lua_element.lua" );
}

std::string_view LuaContext::GetLastError()
{
	return lua_tostring(mLuaState, -1);
}
