#include <catch.hpp>
#include "LuaContext.h"
#include <iostream>

SCENARIO( "How to use lua context", "[LUA_CONTEXT]" )
{
	LuaContext context;

	GIVEN("Execute String")
	{
		REQUIRE( context.ExecuteString( "function TEST() return 3 end") );

		THEN("find global variable")
		{
			REQUIRE( context.IsExistGlobalValue( "TEST" ) );
			REQUIRE( context.ExecuteFunction( "TEST" ) );
		}
	}

	GIVEN("Execute File")
	{
		REQUIRE( context.ExecuteFile( "test/script/test_execute_file.lua" ) );

		THEN("find global variable")
		{
			REQUIRE( context.IsExistGlobalValue( "TEST" ) );
			REQUIRE( context.ExecuteFunction( "TEST" ) );
		}
	}

	GIVEN("Execute Ref Function")
	{
		REQUIRE( context.ExecuteString("function TEST() return 3 end") );
		REQUIRE( context.PushGlobalValue( "TEST" ) );
		LuaContextRefId context_ref_id = context.MakeLuaCallback();

		THEN("Call Ref Function")
		{
			REQUIRE( context_ref_id != LuaContext::REF_NIL );
			REQUIRE( context.ExecuteRefFunction( context_ref_id ) );
		}
	}

}