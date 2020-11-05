//
// Created by HaruGakkaP on 2020-07-08.
//

#ifndef GAMEBOY_GAMEBOY_LUABINDING_H
#define GAMEBOY_GAMEBOY_LUABINDING_H

#include <lua.hpp>
#include <memory>
#include <GameboyCPU.h>

namespace HaruCar::Common::Log
{
	class Logger;
}

class LuaImGuiHandler;

void gameboy_lua_binding_imgui_handler( std::shared_ptr<LuaImGuiHandler> & ref_ptr_handler );
void gameboy_lua_binding_logger( std::shared_ptr<HaruCar::Common::Log::Logger> logger );
void gameboy_lua_binding_cpu( std::shared_ptr<GameboyCPU> cpu  );
void gameboy_lua_binding( lua_State * lua_state );


#endif //GAMEBOY_GAMEBOY_LUABINDING_H
