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

		WHEN("Clear Value")
		{
			REQUIRE_NOTHROW(
					runLua(lua_state,
						   R"(buf:Clear())"));

			THEN("Same as Empty")
			{
				REQUIRE_NOTHROW(
						runLua(lua_state,
			 					R"(ResultEmpty = buf:GetViewString() == "")")
						);

				REQUIRE( getValue<bool>(lua_state, "ResultEmpty") );
			}
		}

		WHEN("Reallocation OK")
		{
			REQUIRE_NOTHROW(
					runLua(lua_state,
							R"(buf:Reallocation(10))")
					);

			THEN( "Resize by 10.")
			{
				char _10arr[10] = { 0 };
				REQUIRE(ptr_buf->Size() == 10);
				REQUIRE(memcmp( ptr_buf->Get(), _10arr, 10 ) == 0);
			}
		}

		WHEN("Reallocation Failed")
		{
			REQUIRE_THROWS(
					runLua(lua_state,
						   R"(buf:Reallocation())")
			);

			REQUIRE_THROWS(
					runLua(lua_state,
						   R"(buf:Reallocation(0))")
			);
		}
	}

	GIVEN("REQUIRE ERROR")
	{
		REQUIRE_THROWS(
				runLua( lua_state,
						"buf = StringBuf()"));
	}

	lua_close( lua_state );

	REQUIRE( GetRemainStringBufCount() == 0 ); // 전부 해제되슴?
}