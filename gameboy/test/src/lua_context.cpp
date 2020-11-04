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

	GIVEN( "Many Ref Function" )
	{
		REQUIRE( context.ExecuteString("function TEST() return 3 end\n"
								 "function TEST2() return 4 end\n"
								 "function TEST3() return 5 end\n") );

		context.PushGlobalValue( "TEST" );
		LuaContextRefId context_ref_id_test = context.MakeLuaCallback();

		context.PushGlobalValue( "TEST2" );
		LuaContextRefId context_ref_id_test2 = context.MakeLuaCallback();

		context.PushGlobalValue( "TEST3" );
		LuaContextRefId context_ref_id_test3 = context.MakeLuaCallback();

		REQUIRE( context_ref_id_test != LuaContext::REF_NIL );
		REQUIRE( context_ref_id_test2 != LuaContext::REF_NIL );
		REQUIRE( context_ref_id_test3 != LuaContext::REF_NIL );

		THEN( "Call Ref Functions")
		{
			REQUIRE( context.ExecuteRefFunction( context_ref_id_test, 1 ) );
			REQUIRE(context.GetIntegerFromStack() == 3 );

			REQUIRE( context.ExecuteRefFunction( context_ref_id_test2, 1 ) );
			REQUIRE(context.GetIntegerFromStack() == 4 );

			REQUIRE( context.ExecuteRefFunction( context_ref_id_test3, 1 ) );
			REQUIRE(context.GetIntegerFromStack() == 5 );
		}
	}

	GIVEN( "Reset, and Ref Function NIL")
	{
		REQUIRE( context.ExecuteString("function TEST() return 3 end") );
		REQUIRE( context.PushGlobalValue( "TEST" ) );
		LuaContextRefId context_ref_id = context.MakeLuaCallback();
		REQUIRE( context_ref_id != LuaContext::REF_NIL );
		context.Reload();

		THEN("Should NIL")
		{
			REQUIRE( !context.IsCorrectKey( context_ref_id ) );
		}
	}
}