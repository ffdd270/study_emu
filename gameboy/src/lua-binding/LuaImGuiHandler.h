#ifndef GAMEBOY_LUAIMGUIHANDLER_H
#define GAMEBOY_LUAIMGUIHANDLER_H

#include "LuaImGuiViewer.h"
#include <vector>
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

	void NoUseImGui();
	void CleanUp();
	void CleanUpOnLuaReload();

	[[nodiscard]] bool IsRenderFailed() const;
	[[nodiscard]] std::string GetRenderFailedReason( std::string_view window_name ) const;
	[[nodiscard]] std::vector<std::string> GetRenderFailedReasons() const;
private:
	std::list<LuaImGuiViewer> mViewers;
	std::shared_ptr<LuaContext> mPtrLuaContext;

	bool mNoUseImGui = false;
	bool mRenderFailed = false;
};


#endif //GAMEBOY_LUAIMGUIHANDLER_H
