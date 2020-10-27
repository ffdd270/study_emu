//
// Created by HaruGakkaP on 2020-07-08.
//

#include "gameboy_luabinding.h"
#include <LuaBridge/LuaBridge.h>
#include "GameboyCPU.h"


void gameboy_lua_binding(lua_State *lua_state)
{
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
}

