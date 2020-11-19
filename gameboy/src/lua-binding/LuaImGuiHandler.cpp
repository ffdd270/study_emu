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

LuaImGuiViewer * LuaImGuiHandler::GetViewer(std::string_view viewer_name)
{
	for( auto & ref_viewer: mViewers )
	{
		if( ref_viewer.GetName() == viewer_name )
		{
			return &ref_viewer;
		}
	}

	return nullptr;
}


void LuaImGuiHandler::Render(const std::shared_ptr<HaruCar::Base::Interface::Provider>& provider_ptr,
							 const std::shared_ptr<HaruCar::UI::Structure::UIEventProtocol>& protocol_ptr)
{
	for ( auto & viewer : mViewers )
	{
		viewer.NoUseImGui( mNoUseImGui );
		viewer.Render(provider_ptr, protocol_ptr);
	}
}

void LuaImGuiHandler::NoUseImGui()
{
	mNoUseImGui = true;
}

/*
 * 
 */

void LuaImGuiHandler::CleanUp()
{
	// 모든 잘못될 가능성에 대한삭제.
	mViewers.remove_if( [](LuaImGuiViewer & ref_viewer )->bool{
		using ContextStatus = LuaImGuiViewer::Status;

		auto status = ref_viewer.GetStatus();

		return
				status == ContextStatus::LUA_REF_ID_INVALID ||
				status == ContextStatus::LUA_CONTEXT_NULL ||
				status == ContextStatus::LUA_CALL_FAILED ||
				status == ContextStatus::LUA_REF_ID_NIL;

	});
}

void LuaImGuiHandler::CleanUpOnLuaReload()
{
	for ( auto & viewer : mViewers )
	{
		viewer.CheckLuaContextValid();
	}

	CleanUp();
}


bool LuaImGuiHandler::IsRenderFailed() const
{
	for( const auto & viewer : mViewers )
	{
		if (viewer.IsRenderFailed()) { return true; }
	}

	return false;
}


std::string LuaImGuiHandler::GetRenderFailedReason(std::string_view window_name) const
{
	for (const auto &viewer : mViewers)
	{
		if ( viewer.GetName() == window_name && viewer.IsRenderFailed() )
		{
			return std::string( viewer.GetLastError() );
		}
	}

	return "";
}

std::vector<std::string> LuaImGuiHandler::GetRenderFailedReasons() const
{
	std::vector<std::string> error_list;

	for (const auto &viewer : mViewers)
	{
		if ( viewer.IsRenderFailed() )
		{
			error_list.emplace_back( viewer.GetLastError() );
		}
	}

	return error_list;
}
