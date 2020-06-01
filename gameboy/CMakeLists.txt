cmake_minimum_required(VERSION 3.15)
project(gameboy)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(gl3w CONFIG REQUIRED)

find_path( GLFW_INCLUDE_DIR GLFW/glfw3.h )

include_directories(${GLFW_INCLUDE_DIR} src/lib/imgui )


set( IMGUI_LIB
		src/lib/imgui/imgui.cpp
		src/lib/imgui/imgui.h
		src/lib/imgui/imgui_draw.cpp
		src/lib/imgui/imgui_internal.h
		src/lib/imgui/imgui_widgets.cpp
		src/lib/imgui/imstb_rectpack.h
		src/lib/imgui/imstb_textedit.h
		src/lib/imgui/imstb_truetype.h
		)

set( DISPLAY_LIB
		src/lib/Window.cpp
		src/lib/Window.h
		src/lib/GLRenderer.cpp
		src/lib/GLRenderer.h
		src/lib/typedef.h

		${IMGUI_LIB} src/lib/ImGuiContext.cpp src/lib/ImGuiContext.h src/lib/Log.h)

add_executable(gameboy src/main.cpp ${DISPLAY_LIB})
target_link_libraries(gameboy PRIVATE glfw OpenGL32 unofficial::gl3w::gl3w)