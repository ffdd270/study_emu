cmake_minimum_required(VERSION 3.17)
project(harucar)

set(CMAKE_CXX_STANDARD 14)
set(HARUCAR_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
		include
		lib/imgui
)

set( LIB_SRC
		src/library.cpp
		include/library.h
		include/CPUViewer.h
		include/CPUProvider.h
		include/Viewer.h
		include/Provider.h
		src/CPUViewer.cpp
		src/CPUProvider.cpp
		src/element/cpu_element.cpp include/element/cpu_element.h)


add_library(harucar ${LIB_SRC})
add_subdirectory(demo)