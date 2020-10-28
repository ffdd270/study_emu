//
// Created by HaruGakkaP on 2020-07-08.
//

#include "gameboy_luabinding.h"
#include <LuaBridge/LuaBridge.h>

#include <utility>
#include "GameboyCPU.h"

static std::shared_ptr<HaruCar::Common::Log::Logger> StaticLuaLoggerInstance = nullptr;

void gameboy_lua_binding_logger(std::shared_ptr<HaruCar::Common::Log::Logger> logger)
{
	StaticLuaLoggerInstance = std::move(logger);
}

HaruCar::Common::Log::Logger * GetInstanceLogger()
{
	if ( StaticLuaLoggerInstance == nullptr ) { return nullptr; }
	return StaticLuaLoggerInstance.get();
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

void gameboy_lua_binding(lua_State *lua_state)
{
	luabridge::getGlobalNamespace(lua_state)
		.addFunction( "GetInstanceLogger", &GetInstanceLogger );

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

	luabridge::getGlobalNamespace(lua_state).
		beginClass<HaruCar::Common::Log::Logger>("Logger")
		.endClass();

	// 로그를 실제로 남길 때는 Global function으로.
	luabridge::getGlobalNamespace(lua_state)
		.addFunction( "log_error", log_error )
		.addFunction( "log_warning", log_warning )
		.addFunction( "log_info", log_info );
}

