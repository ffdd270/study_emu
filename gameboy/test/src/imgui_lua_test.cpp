#include <catch.hpp>
#include "LuaImGuiHandler.h"
#include "gameboy_luabinding.h"

SCENARIO("How to bind to imgui lua test.")
{
	std::shared_ptr<LuaContext> context = std::make_shared<LuaContext>();
	std::shared_ptr<LuaImGuiHandler> ptr_imgui_handler = std::make_shared<LuaImGuiHandler>( context );

	gameboy_lua_binding_imgui_handler( ptr_imgui_handler );

	GIVEN("Add Viewer Call" )
	{
		std::string_view lua_code_execute =
				"AddViewer(\"Window\", function() print(3) end )";

		REQUIRE_NOTHROW( context->ExecuteString( lua_code_execute ) );

		THEN("Window Added")
		{
			REQUIRE( ptr_imgui_handler->GetViewer( "Window" ) != nullptr );
		}
	}
}