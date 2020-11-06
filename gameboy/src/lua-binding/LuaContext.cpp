//
// Created by nhy20 on 2020-10-29.
//

#include "LuaContext.h"
#include "gameboy_luabinding.h"


LuaContext::LuaContext()
{
	init();
}

bool LuaContext::ExecuteFunction(std::string_view func_name, size_t expect_rtn, size_t expect_param )
{
	lua_getglobal( mLuaState, func_name.data() );

	if( !lua_isfunction( mLuaState, lua_gettop( mLuaState ) ) )
	{
		return false;
	}

	return ( lua_pcall( mLuaState, expect_param, expect_rtn, 0 ) == 0 );
}

bool LuaContext::ExecuteRefFunction(LuaContextRefId ref_id, size_t expect_rtn, size_t expect_param )
{
	if ( !IsCorrectKey( ref_id ) )
	{
		mLastError = "Not Correct Key.";
		return false;
	}

	if ( mReferenceMap.find( ref_id ) == mReferenceMap.end() )
	{
		mLastError = "Not in ReferenceMap.";
		return false;
	}

	LuaRefId lua_ref_id = mReferenceMap[ ref_id ];
	lua_rawgeti( mLuaState, LUA_REGISTRYINDEX, lua_ref_id );


	return	( lua_pcall( mLuaState, expect_param, expect_rtn, 0 ) == 0 );
}

int LuaContext::GetIntegerFromStack()
{
	return lua_tointeger( mLuaState, -1 );
}

const char *LuaContext::GetStringFromStack()
{
	return lua_tostring( mLuaState, -1 );
}

size_t LuaContext::GetStackSize()
{
	return lua_gettop( mLuaState );
}

bool LuaContext::ExecuteString(std::string_view execute_string)
{
	if ( luaL_dostring( mLuaState, execute_string.data() ) ) // Return 있으면 문제가..
	{
		return false;
	}

	return true;
}


bool LuaContext::ExecuteFile(std::string_view file_path )
{
	if (luaL_dofile(mLuaState, file_path.data()))
	{
		return false;
	}

	return true;
}

bool LuaContext::PushGlobalValue( std::string_view value_name )
{
	lua_getglobal( mLuaState, value_name.data() );
	if ( lua_isnil( mLuaState, -1 ) ) { return false; }
	return true;
}

bool LuaContext::IsExistGlobalValue(std::string_view value_name)
{
	lua_getglobal( mLuaState, value_name.data() );
	bool rtn = lua_isnil( mLuaState, -1 );
	lua_pop( mLuaState, 1 );
	return !rtn;
}

LuaContextRefId LuaContext::contextRefIdGenerate()
{
	uint64_t key = mReloadCnt & 0xffffu;
	uint64_t unique_id = mRefCnt & 0x0000ffffffffu;

	// 32비트 점프.
	uint64_t ref_id = ( key << 4u * 8u ) | unique_id;

	mRefCnt++;

	return ref_id;
}

bool LuaContext::IsCorrectKey(LuaContextRefId ref_id) const
{
	uint32_t key = ( ref_id & 0xffff00000000u ) >> 32u; //상위 16비트
	uint32_t expect_key = mReloadCnt & 0xffffu;

	return key == expect_key;
}

LuaContextRefId LuaContext::MakeLuaCallback()
{
	LuaRefId ref_id = luaL_ref( mLuaState, LUA_REGISTRYINDEX );
	if ( ref_id == LUA_REFNIL ) { return REF_NIL; }

	LuaContextRefId context_ref_id = contextRefIdGenerate();
	mReferenceMap.insert( std::make_pair( context_ref_id, ref_id ) );
	return context_ref_id;
}

void LuaContext::Reload()
{
	lua_close( mLuaState );
	mLuaState = nullptr;
	init();
}

std::string LuaContext::GetLastError()
{
	if ( !mLastError.empty() )
	{
		std::string return_value = mLastError;
		mLastError = "";
		return mLastError;
	}

	const char * last_err = lua_tostring(mLuaState, -1);
	return last_err == nullptr ?  "" : last_err ;
}

void LuaContext::init()
{
	mRefCnt = 0;
	mReloadCnt++;
	mReferenceMap.clear();

	mLuaState = luaL_newstate();
	luaL_openlibs(mLuaState);

	gameboy_lua_binding(mLuaState);
}

LuaContext::~LuaContext()
{
	lua_close( mLuaState );
}
