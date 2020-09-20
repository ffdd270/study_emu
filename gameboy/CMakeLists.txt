cmake_minimum_required(VERSION 3.15)
project(gameboy)

set(CMAKE_CXX_STANDARD 17)
set( PROJECT_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR} )
set( SRC_PATH ${PROJECT_ROOT_PATH}/src )



find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(gl3w CONFIG REQUIRED)

find_path( GLFW_INCLUDE_DIR GLFW/glfw3.h )

include_directories(${GLFW_INCLUDE_DIR}
		src/angelica/imgui
		)

set( IMGUI_LIB
		src/angelica/imgui/imgui.cpp
		src/angelica/imgui/imgui.h
		src/angelica/imgui/imgui_draw.cpp
		src/angelica/imgui/imgui_internal.h
		src/angelica/imgui/imgui_widgets.cpp
		src/angelica/imgui/imstb_rectpack.h
		src/angelica/imgui/imstb_textedit.h
		src/angelica/imgui/imstb_truetype.h
		)

set( ANGELICA_LIB
		src/angelica/Window.cpp
		src/angelica/Window.h
		src/angelica/GLRenderer.cpp
		src/angelica/GLRenderer.h
		src/angelica/typedef.h
		src/angelica/ImGuiContext.cpp
		src/angelica/ImGuiContext.h
		src/angelica/Log.h
		src/angelica/GLProgram.cpp
		src/angelica/GLProgram.h
		src/angelica/glutil.h

		${IMGUI_LIB}
)


# 실제 테스트 돌릴 것들.
set( GAMEBOY_SRC
		${SRC_PATH}/gameboy/GameboyCPU.cpp
		${SRC_PATH}/gameboy/GameboyCPU.h
		${SRC_PATH}/gameboy/typedef.h
		${SRC_PATH}/gameboy/instruction_set/load_instruction.cpp
		${SRC_PATH}/gameboy/instruction_set/arithmetic_instruction.cpp
		${SRC_PATH}/gameboy/instruction_set/cpu_control_instruction.cpp
		${SRC_PATH}/gameboy/instruction_set/rotate_and_shift.cpp)

add_subdirectory(test)
add_executable(gameboy src/main.cpp ${ANGELICA_LIB} ${GAMEBOY_SRC} )
target_link_libraries(gameboy PRIVATE glfw OpenGL32 unofficial::gl3w::gl3w })