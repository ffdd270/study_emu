//
// Created by HaruGakkaP on 2020-11-04.
//

#include "LuaImGuiViewer.h"

void LuaImGuiViewer::SetLuaContext(std::shared_ptr<LuaContext> &lua_context)
{
	mPtrLuaContext = lua_context;
}

void LuaImGuiViewer::AddLuaCallback(LuaContextRefId ref_id)
{
	mRefId = ref_id
}

void LuaImGuiViewer::Render(std::shared_ptr<HaruCar::Base::Interface::Provider> provider_ptr,
							std::shared_ptr<HaruCar::UI::Structure::UIEventProtocol> protocol_ptr)
{
	if ( mPtrLuaContext == nullptr ) { return; }
	if ( mRefId == LuaContext::REF_NIL ) { return; }
	if ( !mPtrLuaContext->IsCorrectKey( mRefId ) )
	{
		mRefId = LuaContext::REF_NIL;
	}

	mPtrLuaContext->ExecuteRefFunction( mRefId );
}
