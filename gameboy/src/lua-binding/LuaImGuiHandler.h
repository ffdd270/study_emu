#ifndef GAMEBOY_LUAIMGUIHANDLER_H
#define GAMEBOY_LUAIMGUIHANDLER_H

#include "LuaImGuiViewer.h"
#include <list>


class LuaImGuiHandler
{
public:
	LuaImGuiHandler( std::shared_ptr<LuaContext> & ref_ptr_lua_context );

	// Stack ( top == 2 )
	// 2 -> Lua Function
	// 1 -> Name
	void MakeViewer();

	void Render(std::shared_ptr<HaruCar::Base::Interface::Provider> provider_ptr,
				std::shared_ptr<HaruCar::UI::Structure::UIEventProtocol> protocol_ptr);
private:
	std::list<LuaImGuiViewer> mViewers;
	std::shared_ptr<LuaContext> mPtrLuaContext;
};


#endif //GAMEBOY_LUAIMGUIHANDLER_H
