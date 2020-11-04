cmake_minimum_required(VERSION 3.15)
project(gameboy)

set(CMAKE_CXX_STANDARD 17)
set( PROJECT_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR} )
set( SRC_PATH ${PROJECT_ROOT_PATH}/src )
set( HARUCAR_PATH ${PROJECT_ROOT_PATH}/../harucar )

include(FindLua)

find_package(SFML 2 COMPONENTS graphics REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Lua REQUIRED)

find_library(HARUCAR_LIB
		NAMES harucar.lib
		HINTS ${HARUCAR_PATH}/cmake-build-debug )

include_directories(
		${HARUCAR_PATH}/lib/imgui/
		${HARUCAR_PATH}/lib/imgui-sfml/
		${HARUCAR_PATH}/include/

		${PROJECT_ROOT_PATH}/external/
		${PROJECT_ROOT_PATH}/src/gameboy/
		${LUA_INCLUDE_DIR}/
)

set(IMGUI
		${HARUCAR_PATH}/lib/imgui/imconfig.h
		${HARUCAR_PATH}/lib/imgui/imgui.cpp
		${HARUCAR_PATH}/lib/imgui/imgui.h
		${HARUCAR_PATH}/lib/imgui/imgui_draw.cpp
		${HARUCAR_PATH}/lib/imgui/imgui_internal.h
		${HARUCAR_PATH}/lib/imgui/imgui_widgets.cpp
		${HARUCAR_PATH}/lib/imgui/imstb_rectpack.h
		${HARUCAR_PATH}/lib/imgui/imstb_textedit.h
		${HARUCAR_PATH}/lib/imgui/imstb_truetype.h)

set(IMGUI_SFML
		${HARUCAR_PATH}/lib/imgui-sfml/imconfig-SFML.h
		${HARUCAR_PATH}/lib/imgui-sfml/imgui-SFML.cpp
		${HARUCAR_PATH}/lib/imgui-sfml/imgui-SFML.h
		${HARUCAR_PATH}/lib/imgui-sfml/imgui-SFML_export.h)

# 실제 테스트 돌릴 것들.
set( GAMEBOY_SRC
		${SRC_PATH}/gameboy/GameboyCPU.cpp
		${SRC_PATH}/gameboy/GameboyCPU.h
		${SRC_PATH}/gameboy/typedef.h

		${SRC_PATH}/gameboy/instruction_set/load_instruction.cpp
		${SRC_PATH}/gameboy/instruction_set/arithmetic_instruction.cpp
		${SRC_PATH}/gameboy/instruction_set/cpu_control_instruction.cpp
		${SRC_PATH}/gameboy/instruction_set/rotate_and_shift.cpp
		${SRC_PATH}/gameboy/instruction_set/bit_set_reset_test.cpp
		${SRC_PATH}/gameboy/instruction_set/jump_group.cpp
		${SRC_PATH}/gameboy/instruction_set/call_and_return.cpp

		${SRC_PATH}/gameboy/GameboyCPUBroker.cpp
		${SRC_PATH}/gameboy/GameboyCPUBroker.h

		${SRC_PATH}/lua-binding/gameboy_luabinding.cpp
		${SRC_PATH}/lua-binding/gameboy_luabinding.h

		${SRC_PATH}/lua-binding/LuaContext.cpp
		${SRC_PATH}/lua-binding/LuaContext.h

		${SRC_PATH}/lua-binding/LuaImGuiHandler.cpp
		${SRC_PATH}/lua-binding/LuaImGuiHandler.h

		${SRC_PATH}/lua-binding/LuaImGuiViewer.cpp
		${SRC_PATH}/lua-binding/LuaImGuiViewer.h)

set( UI_SRC
		${SRC_PATH}/harucar/LuaCommandViewer.cpp
		${SRC_PATH}/harucar/LuaCommandViewer.h
		)


set( COLOR_TEXT_EDIT
		external/color_text_edit/TextEditor.cpp
		external/color_text_edit/TextEditor.h )



add_subdirectory(test)
add_executable(gameboy src/main.cpp ${IMGUI} ${IMGUI_SFML} ${GAMEBOY_SRC} ${COLOR_TEXT_EDIT} ${UI_SRC} )
target_link_libraries(gameboy PRIVATE sfml-graphics OpenGL32 ${HARUCAR_LIB} ${LUA_LIBRARIES})