cmake_minimum_required(VERSION 3.17)
project(harucar)

set(CMAKE_CXX_STANDARD 14)
set(HARUCAR_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
		include
		lib/imgui
)

set( LIB_SRC
		# Interface.
		include/Viewer.h
		include/Provider.h

		include/library.h
		src/library.cpp

		include/CPUViewer.h
		src/CPUViewer.cpp

		include/CPUProvider.h
		src/CPUProvider.cpp

		include/element/cpu_element.h
		src/element/cpu_element.cpp)


add_library(harucar ${LIB_SRC})
add_subdirectory(demo)
add_subdirectory(test)