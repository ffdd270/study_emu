#include <catch.hpp>
#include "GameboyCPU.h"
#include "lua_util.h"

SCENARIO("Usage of StringBuf", "[BIND]")
{
	lua_State * lua_state = luaL_newstate();
	luaL_openlibs( lua_state );

	gameboy_lua_binding( lua_state );

	GIVEN("A Single StringBuf")
	{
		REQUIRE_NOTHROW(
				runLua(lua_state,
					   "buf = StringBuf( 100 )"));

		auto *ptr_buf = getValue<StringBuf *>(lua_state, "buf");
		REQUIRE(ptr_buf->Size() == 100);

		char arr[100] = { 0 };
		REQUIRE(memcmp( ptr_buf->Get(), arr, 100 ) == 0);

		const char * source = "StringBuf";
		strcpy_s( ptr_buf->Get(), ptr_buf->Size(), source );

		WHEN("Get Value From Lua")
		{
			REQUIRE_NOTHROW(
					runLua(lua_state,
						   R"(Result = buf:GetViewString() == "StringBuf")"));


			REQUIRE_NOTHROW(
					runLua(lua_state,
						   R"(ResultFalse = buf:GetViewString() == "StringNotBuf")"));

			THEN("CMP WAS TRUE")
			{
				REQUIRE( getValue<bool>(lua_state, "Result") );
				REQUIRE( getValue<bool>(lua_state, "ResultFalse") == false );
			}
		}
	}

	GIVEN("REQUIRE ERROR")
	{
		REQUIRE_THROWS(
				runLua( lua_state,
						"buf = StringBuf()"));
	}


}