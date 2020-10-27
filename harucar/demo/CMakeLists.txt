cmake_minimum_required(VERSION 3.17)
project(harucar-demo)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2 COMPONENTS graphics REQUIRED)
find_package(OpenGL REQUIRED)


include_directories(
		${HARUCAR_ROOT_PATH}/lib/imgui/
		${HARUCAR_ROOT_PATH}/lib/imgui-sfml/
		${HARUCAR_ROOT_PATH}/header
)

set(IMGUI
		${HARUCAR_ROOT_PATH}/lib/imgui/imconfig.h
		${HARUCAR_ROOT_PATH}/lib/imgui/imgui.cpp
		${HARUCAR_ROOT_PATH}/lib/imgui/imgui.h
		${HARUCAR_ROOT_PATH}/lib/imgui/imgui_draw.cpp
		${HARUCAR_ROOT_PATH}/lib/imgui/imgui_internal.h
		${HARUCAR_ROOT_PATH}/lib/imgui/imgui_widgets.cpp
		${HARUCAR_ROOT_PATH}/lib/imgui/imstb_rectpack.h
		${HARUCAR_ROOT_PATH}/lib/imgui/imstb_textedit.h
		${HARUCAR_ROOT_PATH}/lib/imgui/imstb_truetype.h)

set(IMGUI_SFML
		${HARUCAR_ROOT_PATH}/lib/imgui-sfml/imconfig-SFML.h
		${HARUCAR_ROOT_PATH}/lib/imgui-sfml/imgui-SFML.cpp
		${HARUCAR_ROOT_PATH}/lib/imgui-sfml/imgui-SFML.h
		${HARUCAR_ROOT_PATH}/lib/imgui-sfml/imgui-SFML_export.h)


set(DEMO_SRC
		main.cpp)

add_executable(harucar-demo ${DEMO_SRC} ${IMGUI} ${IMGUI_SFML})

target_link_libraries( harucar-demo PRIVATE harucar sfml-graphics OpenGL32 )