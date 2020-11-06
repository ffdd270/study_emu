#ifndef GAMEBOY_LUAIMGUIHANDLER_H
#define GAMEBOY_LUAIMGUIHANDLER_H

#include "LuaImGuiViewer.h"
#include <list>
#include <optional>


class LuaImGuiHandler
{
public:
	explicit LuaImGuiHandler( std::shared_ptr<LuaContext> & ref_ptr_lua_context );

	// Stack ( top == 2 )
	// 2 -> Lua Function
	// 1 -> Name
	void MakeViewer();

	// Not found == nullptr
	LuaImGuiViewer * GetViewer( std::string_view viewer_name );

	void Render(const std::shared_ptr<HaruCar::Base::Interface::Provider>& provider_ptr,
				const std::shared_ptr<HaruCar::UI::Structure::UIEventProtocol>& protocol_ptr);
private:
	std::list<LuaImGuiViewer> mViewers;
	std::shared_ptr<LuaContext> mPtrLuaContext;
};


#endif //GAMEBOY_LUAIMGUIHANDLER_H
