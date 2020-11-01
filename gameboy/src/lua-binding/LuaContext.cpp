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

	if( !lua_isfunction( mLuaState, lua_gettop( mLuaState ) ) )
	{
		return false;
	}

	return ( lua_pcall( mLuaState, 0, 0, 0 ) == 0 );
}

bool LuaContext::ExecuteString(std::string_view execute_string)
{
	if ( luaL_dostring( mLuaState, execute_string.data() ) ) // Return 있으면 문제가..
	{
		return false;
	}

	return true;
}


bool LuaContext::ExecuteFile(std::string_view file_path )
{
	if (luaL_dofile(mLuaState, file_path.data()))
	{
		return false;
	}

	return true;
}

void LuaContext::init()
{
	mLuaState = luaL_newstate();
	luaL_openlibs( mLuaState );

	gameboy_lua_binding( mLuaState );
}

std::string_view LuaContext::GetLastError()
{
	const char * last_err = lua_tostring(mLuaState, -1);
	return last_err == nullptr ?  "" : last_err ;
}

bool LuaContext::IsExistGlobalValue(std::string_view value_name)
{
	lua_getglobal( mLuaState, value_name.data() );
	bool rtn = lua_isnil( mLuaState, -1 );
	lua_pop( mLuaState, 1 );
	return !rtn;
}

void LuaContext::Reload()
{
	lua_close( mLuaState );
	mLuaState = nullptr;
	init();
}


LuaContext::~LuaContext()
{
	lua_close( mLuaState );
}
