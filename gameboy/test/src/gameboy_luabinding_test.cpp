//
// Created by HaruGakkaP on 2020-10-22.
//

#include <catch.hpp>
#include "GameboyCPU.h"
#include "gameboy_luabinding.h"
#include <LuaBridge/LuaBridge.h>

void runLua(lua_State * L, const std::string& script)
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

template <typename T>
T getValue(lua_State * L, const std::string & value_name )
{
	return luabridge::getGlobal(L, value_name.c_str()).cast<T>();
}

GameboyCPU getCPU()
{
	return GameboyCPU();
}

void test_helper_lua_functions( lua_State * lua_state )
{
	luabridge::getGlobalNamespace(lua_state)
		.addFunction( "getCPU", &getCPU );
}


TEST_CASE("TEST GAMEBOY ON LUA", "[LUA]")
{
	lua_State * lua_state = luaL_newstate();
	luaL_openlibs(lua_state);
	gameboy_lua_binding( lua_state );
	test_helper_lua_functions( lua_state );

	SECTION("basic register get test.")
	{
		REQUIRE_NOTHROW( runLua( lua_state, "HL = getCPU():GetRegisterHL()" ) );

		auto result_value = getValue<Register>( lua_state, "HL" );

		REQUIRE( result_value.reg_16 == 0xffff );
		REQUIRE( result_value.hi == 0xff );
		REQUIRE( result_value.lo == 0xff );
	}

	lua_close(lua_state);
}