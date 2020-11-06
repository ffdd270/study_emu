//
// Created by HaruGakkaP on 2020-11-04.
//

#include "LuaImGuiViewer.h"


LuaImGuiViewer::LuaImGuiViewer(std::string_view name, std::shared_ptr<LuaContext> &ref_ptr_lua_context, LuaContextRefId context_ref_id)
{
	SetName( name );
	SetLuaContext( ref_ptr_lua_context );
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

std::string_view LuaImGuiViewer::GetName() const
{
	return mWindowName;
}

std::string_view LuaImGuiViewer::GetLastError() const
{
	return mLastError;
}

bool LuaImGuiViewer::IsRenderFailed() const
{
	return mRenderFailed;
}

void LuaImGuiViewer::AddLuaCallback(LuaContextRefId ref_id)
{
	mRefId = ref_id;
}

void LuaImGuiViewer::Render(std::shared_ptr<HaruCar::Base::Interface::Provider> provider_ptr,
							std::shared_ptr<HaruCar::UI::Structure::UIEventProtocol> protocol_ptr)
{
	mRenderFailed = true;

	if (mPtrLuaContext == nullptr)
	{
		mStatus = Status::LUA_CONTEXT_NULL;
		return;
	}
	if (mRefId == LuaContext::REF_NIL)
	{
		mStatus = Status::LUA_REF_ID_NIL;
		return;
	}
	if (!mPtrLuaContext->IsCorrectKey(mRefId))
	{
		mStatus = Status::LUA_REF_ID_INVALID;
		mRefId = LuaContext::REF_NIL;
		return;
	}

	bool ok = mPtrLuaContext->ExecuteRefFunction(mRefId);
	if (!ok)
	{
		mStatus = Status::LUA_CALL_FAILED;
		mLastError = mPtrLuaContext->GetLastError();
		return;
	}

	mRenderFailed = false;
}

LuaImGuiViewer::Status LuaImGuiViewer::GetStatus() const
{
	return mStatus;
}
