cmake_minimum_required(VERSION 3.15)
project(gb_test)
set(CMAKE_CXX_STANDARD 17)

include_directories( lib/
        ${SRC_PATH}/gameboy/
		${SRC_PATH}/lua-binding/
		${HARUCAR_PATH}/include/
		)

set(TEST
		src/util.h
		src/main.cpp
        src/register.cpp
        src/load_instruction_set.cpp
        src/arithmetic_instruction_set.cpp
		src/cpu_control_instruction.cpp
		src/rotate_and_shift.cpp
		src/bit_set_reset_test.cpp
		src/jump_group.cpp
		src/call_and_return.cpp
		src/gameboy_broker.cpp
		src/gameboy_luabinding_test.cpp 
		src/breakpoint.cpp
		src/lua_context.cpp
		src/imgui_lua_test.cpp
		src/cpu_public_methods.cpp
		src/test_for_lua_bind_class.cpp
		src/lua_util.h)


add_executable( gb_test
        lib/catch.hpp
        ${TEST}
        ${GAMEBOY_SRC}
		${IMGUI})

target_link_libraries(gb_test PRIVATE ${HARUCAR_LIB} ${LUA_LIBRARIES})