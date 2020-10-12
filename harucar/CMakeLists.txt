cmake_minimum_required(VERSION 3.17)
project(harucar)

set(CMAKE_CXX_STANDARD 14)
set(HARUCAR_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
		header
		lib/imgui
)

set( LIB_SRC
		src/library.cpp
		header/library.h
		header/CPUViewer.h
		header/CPUProvider.h
		header/Viewer.h
		header/Provider.h
		src/CPUViewer.cpp
		src/CPUProvider.cpp
		src/element/cpu_element.cpp header/element/cpu_element.h)


add_library(harucar ${LIB_SRC})
add_subdirectory(demo)