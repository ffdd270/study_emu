#include <catch.hpp>
#include "LuaContext.h"


SCENARIO( "How to use lua context", "[LUA_CONTEXT]" )
{

	GIVEN("Execute String")
	{
		LuaContext context;
		REQUIRE( context.ExecuteString( "function TEST() return 3 end") );

		THEN("find global variable")
		{
			REQUIRE( context.IsExistGlobalValue( "TEST" ) );
			REQUIRE( context.ExecuteFunction( "TEST" ) );
		}
	}

	GIVEN("Execute File")
	{
		LuaContext context;
		REQUIRE( context.ExecuteFile( "test/script/test_execute_file.lua" ) );

		THEN("find global variable")
		{
			REQUIRE( context.IsExistGlobalValue( "TEST" ) );
			REQUIRE( context.ExecuteFunction( "TEST" ) );
		}
	}

}