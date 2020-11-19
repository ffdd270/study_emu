//
// Created by HaruGakkaP on 2020-11-04.
//

#ifndef GAMEBOY_LUAIMGUIVIEWER_H
#define GAMEBOY_LUAIMGUIVIEWER_H

#include <base/viewer_interface.h>
#include "LuaContext.h"



class LuaImGuiViewer : public HaruCar::Base::Interface::Viewer
{
public:
	enum class Status
	{
		LUA_CONTEXT_NULL,
		LUA_REF_ID_NIL,
		LUA_REF_ID_INVALID,
		LUA_CALL_FAILED,
		OK = 9999
	};


	LuaImGuiViewer( std::string_view name, std::shared_ptr<LuaContext> & ref_ptr_lua_context, LuaContextRefId context_ref_id );
	void SetLuaContext( std::shared_ptr<LuaContext> & lua_context );
	void SetName( std::string_view name );
	void NoUseImGui( bool flag );

	[[nodiscard]] std::string_view GetName() const;
	[[nodiscard]] std::string_view GetLastError() const;
	[[nodiscard]] Status GetStatus() const;

	bool CheckLuaContextValid();
 	[[nodiscard]] bool IsRenderFailed() const;

	void AddLuaCallback(LuaContextRefId ref_id);

	void Render(std::shared_ptr<HaruCar::Base::Interface::Provider> provider_ptr,
			 std::shared_ptr<HaruCar::UI::Structure::UIEventProtocol> protocol_ptr) final;
private:
	LuaContextRefId mRefId = LuaContext::REF_NIL;

	Status mStatus = Status::OK;

	std::string mLastError;
	std::string mWindowName;
	bool mRenderFailed = false;
	bool mNoUseImGui = false;

	std::shared_ptr<LuaContext> mPtrLuaContext = nullptr;
};

#endif //GAMEBOY_LUAIMGUIVIEWER_H
