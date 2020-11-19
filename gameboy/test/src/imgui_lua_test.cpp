#include <catch.hpp>
#include "LuaImGuiHandler.h"
#include "gameboy_luabinding.h"
#include "common/common_logger.h"

#include <iostream>
SCENARIO("How to bind to imgui lua test.")
{
	std::shared_ptr<HaruCar::Common::Log::Logger> ptr_logger = std::make_shared<HaruCar::Common::Log::Logger>();
	std::shared_ptr<LuaContext> ptr_context = std::make_shared<LuaContext>();
	std::shared_ptr<LuaImGuiHandler> ptr_imgui_handler = std::make_shared<LuaImGuiHandler>( ptr_context );

	gameboy_lua_binding_logger( ptr_logger );
	gameboy_lua_binding_imgui_handler( ptr_imgui_handler );

	GIVEN("Add Viewer Call" )
	{
		std::string_view lua_code_execute =
				"AddViewer(\"Window\", function() print(3) end )";

		REQUIRE_NOTHROW(ptr_context->ExecuteString(lua_code_execute ) );

		THEN("Window Added")
		{
			REQUIRE( ptr_imgui_handler->GetViewer( "Window" ) != nullptr );
		}
	}

	GIVEN("Rendering Lua Functions")
	{
		std::string_view lua_code_execute =
				R"( AddViewer("Render", function() log_info(GetInstanceLogger(), "Executed.") end) )";

		REQUIRE_NOTHROW( ptr_context->ExecuteString(lua_code_execute ) );
		REQUIRE( ptr_imgui_handler->GetViewer( "Render" ) != nullptr );

		WHEN("Render Call")
		{
			ptr_imgui_handler->NoUseImGui();
			ptr_imgui_handler->Render( nullptr, nullptr );
			ptr_imgui_handler->CleanUp();

			THEN("Logger Info added Executed")
			{
				REQUIRE_FALSE( ptr_imgui_handler->IsRenderFailed() );
				REQUIRE( ptr_logger->GetSize() == 1 );
				REQUIRE( ptr_logger->GetData(0).log == "Executed." );

				// And, OK Get Viewer.
				REQUIRE( ptr_imgui_handler->GetViewer("Render") != nullptr );
			}
		}

		WHEN("Reload after Render Call.")
		{
			ptr_context->Reload();
			ptr_imgui_handler->Render( nullptr, nullptr );

			THEN( "Render Failed, Killed Viewer." )
			{
				REQUIRE( ptr_imgui_handler->IsRenderFailed() );

				ptr_imgui_handler->CleanUp(); // 에러 확인 후 정리.

				REQUIRE( ptr_imgui_handler->GetViewer("Render") == nullptr );
				REQUIRE( ptr_logger->GetSize() == 0 );
			}
		}
	}

	GIVEN("Invalid Lua Context's two, OK Lua Context One.")
	{
		std::string_view lua_code_execute =
				R"(
					AddViewer("Render1", function() log_info() end) -- Wrong Param.
					AddViewer("Render2", function() log_info(GetInstanceLogger()) end) -- Wrong Param.
					AddViewer("Render3", function() log_info(GetInstanceLogger(), "Executed.") end) -- OK
				)";

		REQUIRE_NOTHROW( ptr_context->ExecuteString(lua_code_execute ) );
		std::cout << ptr_context->GetLastError();

		REQUIRE( ptr_imgui_handler->GetViewer( "Render1" ) != nullptr );
		REQUIRE( ptr_imgui_handler->GetViewer( "Render2" ) != nullptr );
		REQUIRE( ptr_imgui_handler->GetViewer( "Render3" ) != nullptr );

		WHEN("Render!")
		{
			ptr_imgui_handler->NoUseImGui();
			REQUIRE_NOTHROW( ptr_imgui_handler->Render( nullptr, nullptr ) );
			ptr_imgui_handler->CleanUp();

			THEN("Log should one, Render Should One.")
			{
				REQUIRE( ptr_imgui_handler->GetViewer( "Render1" ) == nullptr );
				REQUIRE( ptr_imgui_handler->GetViewer( "Render2" ) == nullptr );
				REQUIRE( ptr_imgui_handler->GetViewer( "Render3" ) != nullptr );

				REQUIRE( ptr_logger->GetSize() == 1 );
				REQUIRE( ptr_logger->GetData(0).log == "Executed." );
			}
		}
	}
}