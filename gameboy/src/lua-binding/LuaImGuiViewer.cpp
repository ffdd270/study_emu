//
// Created by HaruGakkaP on 2020-11-04.
//

#include "LuaImGuiViewer.h"


LuaImGuiViewer::LuaImGuiViewer(std::string_view name, std::shared_ptr<LuaContext> &ref_ptr_lua_context, LuaContextRefId context_ref_id)
{
	SetName( name );
	SetLuaContext( mPtrLuaContext );
	AddLuaCallback( context_ref_id );

}

void LuaImGuiViewer::SetLuaContext(std::shared_ptr<LuaContext> &lua_context)
{
	mPtrLuaContext = lua_context;
}

void LuaImGuiViewer::SetName(std::string_view name)
{
	mWindowName = name;
}

std::string_view LuaImGuiViewer::GetName()
{
	return mWindowName;
}

void LuaImGuiViewer::AddLuaCallback(LuaContextRefId ref_id)
{
	mRefId = ref_id;
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
