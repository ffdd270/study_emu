//
// Created by nhy20 on 2020-10-29.
//

#ifndef GAMEBOY_LUACONTEXT_H
#define GAMEBOY_LUACONTEXT_H

#include <lua.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>

typedef size_t LuaContextRefId;
typedef int LuaRefId;

// Reload를 ffff번 하고 Callback 바인딛 하면 ... 망가지지는 않을 것 같은데.. :thinking_face:
class LuaContext
{
public:
	static const LuaContextRefId REF_NIL = 0x1000000000000;

	LuaContext();

	bool ExecuteFunction( std::string_view func_name );
	bool ExecuteRefFunction( LuaContextRefId ref_id );

	bool ExecuteString( std::string_view execute_string );
	bool ExecuteFile( std::string_view file_path );

	bool PushGlobalValue( std::string_view value_name );

	bool IsExistGlobalValue( std::string_view value_name );

	// 48비트 키 제너레이트.
	// 초반 ffff, => reload cnt.
	// 나머지,
	// reload cnt를 높일까, 아니면 key를 재활용할까.. 고민중.
	[[nodiscard]] bool IsCorrectKey(LuaContextRefId ref_id ) const;

	// On After Push
	LuaContextRefId MakeLuaCallback();

	void Reload();

	std::string GetLastError();

	~LuaContext();
private:
	[[nodiscard]] LuaContextRefId contextRefIdGenerate();
	void init();
private:
	std::unordered_map<LuaContextRefId, LuaRefId> mReferenceMap;
	std::string mLastError;

	size_t mReloadCnt = 0;
	size_t mRefCnt = 0;

	lua_State * mLuaState = nullptr;
};


#endif //GAMEBOY_LUACONTEXT_H
