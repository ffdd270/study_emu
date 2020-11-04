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

class LuaContext
{
public:
	LuaContext();

	bool ExecuteFunction( std::string_view func_name );
	bool ExecuteString( std::string_view execute_string );
	bool ExecuteFile( std::string_view file_path );

	bool IsExistGlobalValue( std::string_view value_name );

	// Need..
	// lua_stack[ stack ] == lua_function
	LuaContextRefId MakeLuaCallback( int stack );

	void Reload();

	std::string_view GetLastError();

	~LuaContext();
private:
	// 48비트 키 제너레이트.
	// 초반 ffff, => reload cnt.
	// 나머지,
	// reload cnt를 높일까, 아니면 key를 재활용할까.. 고민중.
	[[nodiscard]] LuaContextRefId contextRefIdGenerate() const;
	[[nodiscard]] bool isCurrentKey( LuaContextRefId ref_id ) const;
	void init();
private:
	std::unordered_map<LuaContextRefId, LuaRefId> mReferenceMap;

	size_t mReloadCnt = 0;
	size_t mRefCnt = 0;
	lua_State * mLuaState = nullptr;
};


#endif //GAMEBOY_LUACONTEXT_H
