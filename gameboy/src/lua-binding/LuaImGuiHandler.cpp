//
// Created by HaruGakkaP on 2020-11-04.
//

#include "LuaImGuiHandler.h"
#include <exception>
#include <stdexcept>

LuaImGuiHandler::LuaImGuiHandler(std::shared_ptr<LuaContext> &ref_ptr_lua_context) : mPtrLuaContext( ref_ptr_lua_context )
{
	if( ref_ptr_lua_context == nullptr ) { throw std::logic_error("Ptr Was Nullptr!"); }
}

void LuaImGuiHandler::MakeViewer()
{
	if ( mPtrLuaContext == nullptr )  { throw std::logic_error("Ptr was Nullptr!"); }
	if ( mPtrLuaContext->GetStackSize() != 2 ) { throw std::logic_error("Wrong Call Params."); }

	if (!mPtrLuaContext->IsTypeOf<LuaContext::LuaSpecialTypes::LUA_FUNC>()) // function?
	{
		throw std::logic_error("Not Lua function. check param.");
	}

	if (!mPtrLuaContext->IsTypeOf<const char *>( -2 )) // string?
	{
		throw std::logic_error("Not String. Check Param.");
	}

	LuaContextRefId context_ref_id = mPtrLuaContext->MakeLuaCallback();
	std::string_view name = mPtrLuaContext->GetStringFromStack();

	mViewers.emplace_back( LuaImGuiViewer( name, mPtrLuaContext, context_ref_id ) );
}

void LuaImGuiHandler::Render(std::shared_ptr<HaruCar::Base::Interface::Provider> provider_ptr,
							 std::shared_ptr<HaruCar::UI::Structure::UIEventProtocol> protocol_ptr)
{
	for ( auto & viewer : mViewers )
	{
		viewer.Render( provider_ptr, protocol_ptr );
	}
}
