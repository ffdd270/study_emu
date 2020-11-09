//
// Created by HaruGakkaP on 2020-07-08.
//

#include "gameboy_luabinding.h"
#include <LuaBridge/LuaBridge.h>

#include <utility>
#include <stdexcept>
#include <imgui.h>
#include "GameboyCPU.h"
#include "LuaImGuiHandler.h"
#include "LuaBridge/Vector.h"

#include <common/common_logger.h>

static std::shared_ptr<LuaImGuiHandler> StaticLuaImGuiHandler = nullptr;
static std::shared_ptr<HaruCar::Common::Log::Logger> StaticLuaLoggerInstance = nullptr;
static std::shared_ptr<GameboyCPU> StaticGameboyCPUInstance = nullptr;

void gameboy_lua_binding_imgui_handler(std::shared_ptr<LuaImGuiHandler> & ref_ptr_handler)
{
	StaticLuaImGuiHandler = ref_ptr_handler;
}

void gameboy_lua_binding_logger(std::shared_ptr<HaruCar::Common::Log::Logger> logger)
{
	StaticLuaLoggerInstance = std::move(logger);
}

void gameboy_lua_binding_cpu(std::shared_ptr<GameboyCPU> cpu)
{
	StaticGameboyCPUInstance = std::move(cpu);
}

HaruCar::Common::Log::Logger * GetInstanceLogger()
{
	if ( StaticLuaLoggerInstance == nullptr ) { return nullptr; }
	return StaticLuaLoggerInstance.get();
}

GameboyCPU * GetInstanceCPU()
{
	if ( StaticGameboyCPUInstance == nullptr ) { return nullptr; }
	return StaticGameboyCPUInstance.get();
}


using namespace HaruCar::Common::Log;


static void log_error( Logger * logger, const char * str  )
{
	logger->LogError( str );
}

static void log_warning( Logger * logger, const char * str  )
{
	logger->LogWarn( str );
}

static void log_info( Logger * logger, const char * str  )
{
	logger->LogInfo( str );
}

// Imgui Warrppher

struct ImDrawList_Warp
{
public:
	friend ImDrawList_Warp * ImGui_GetWindowDrawList();

	void AddText( const ImVec2 pos, uint32_t color, const char * str )
	{
		mDrawList->AddText(  pos, color, str );

	}
private:
	void setDrawList( ImDrawList * ptr_draw_list ) { mDrawList = ptr_draw_list; }
private:
	ImDrawList * mDrawList = nullptr;
};

static std::shared_ptr<ImDrawList_Warp> StaticImDrawList = nullptr;

void ImGui_Begin( const char * window_name )
{
	ImGui::Begin( window_name );
}

void ImGui_Text( const char * str )
{
	ImGui::Text( "%s", str );
}

ImDrawList_Warp * ImGui_GetWindowDrawList( )
{
	ImDrawList * draw_list_ptr = ImGui::GetWindowDrawList();

	if ( StaticImDrawList == nullptr )
	{
		StaticImDrawList = std::make_shared<ImDrawList_Warp>();
	}

	StaticImDrawList->setDrawList( draw_list_ptr );

	return StaticImDrawList.get();
}

void ImGui_SameLine()
{
	ImGui::SameLine();
}

int lua_AddViewer( lua_State * lua_state )
{
	if ( StaticLuaImGuiHandler == nullptr ) { throw std::logic_error("No ImGuiHandler."); }

	StaticLuaImGuiHandler->MakeViewer();

	return 0;
}


int lua_get_last_logs( lua_State * lua_state )
{
	Logger * ptr_logger = luabridge::Stack<Logger *>::get( lua_state, -1 );

	if ( ptr_logger->GetLogSizeFromLastGet() == 0 )
	{
		return 0;
	}

	luabridge::Stack<std::vector<LogData>>::push( lua_state, ptr_logger->GetLogsFromLastGet() );
	return 1;
}


void gameboy_lua_binding(lua_State *lua_state)
{
	luabridge::getGlobalNamespace(lua_state)
		.addFunction( "GetInstanceLogger", &GetInstanceLogger )
		.addFunction( "GetInstanceCPU", &GetInstanceCPU );

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<GameboyCPU>("GameboyCPU")
			.addFunction( "InjectionMemory", &GameboyCPU::InjectionMemory )
			.addFunction( "GetMemoryValue", &GameboyCPU::GetMemoryValue )
			.addFunction( "GetRegisterAF", &GameboyCPU::GetRegisterAF )
			.addFunction( "GetRegisterBC", &GameboyCPU::GetRegisterBC )
			.addFunction( "GetRegisterDE", &GameboyCPU::GetRegisterDE )
			.addFunction( "GetRegisterHL", &GameboyCPU::GetRegisterHL )
		.endClass();

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<Register>("Register")
		        .addData( "reg_16", &Register::reg_16 )
		        .addData( "hi", &Register::hi )
		        .addData( "lo", &Register::lo )
		.endClass();

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<LogData>("LogData")
		        .addData( "log", &LogData::log )
		        .addData( "info", &LogData::info )
		.endClass();

	luabridge::getGlobalNamespace(lua_state).
		beginClass<HaruCar::Common::Log::Logger>("Logger")
		.endClass();

	// Imgui
	luabridge::getGlobalNamespace(lua_state)
		.beginClass<ImVec2>("ImVec2")
		        .addConstructor<void (*)( float, float )>()
		                .addData( "x", &ImVec2::x )
		                .addData( "y", &ImVec2::y )
		        .endClass();

	luabridge::getGlobalNamespace(lua_state)
		.beginClass<ImDrawList_Warp>("ImDrawList")
		        .addFunction( "AddText", &ImDrawList_Warp::AddText )
		.endClass();

	luabridge::getGlobalNamespace(lua_state)
		.beginNamespace("ImGui")
			.addFunction( "Begin", &ImGui_Begin)
			.addFunction( "End", &ImGui::End)
			.addFunction( "GetWindowDrawList", ImGui_GetWindowDrawList)
			.addFunction( "Text", &ImGui_Text )
			.addFunction( "SameLine", &ImGui_SameLine )
		.endNamespace();

	// 로그를 실제로 남길 때는 Global function으로.
	luabridge::getGlobalNamespace(lua_state)
		.addFunction( "log_error", log_error )
		.addFunction( "log_warning", log_warning )
		.addFunction( "log_info", log_info )
		.addCFunction( "get_last_logs", lua_get_last_logs )
		.addCFunction( "AddViewer", lua_AddViewer );
}


