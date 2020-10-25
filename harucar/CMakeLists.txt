cmake_minimum_required(VERSION 3.17)
project(harucar)

set(CMAKE_CXX_STANDARD 17)
set(HARUCAR_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
		include
		lib/imgui
)

set( LIB_SRC
		# Interface.
		include/base/viewer.h
		include/base/provider.h

		include/library.h
		src/library.cpp

		include/cpu/cpu_viewer.h
		src/cpu/cpu_viewer.cpp

		include/cpu/cpu_provider.h
		src/cpu/cpu_provider.cpp

		include/cpu/cpu_element.h
		src/cpu/cpu_element.cpp

		include/common/common_element.h
		src/common/common_element.cpp

		include/common/common_structure.h
		src/common/common_structure.cpp

		include/ui/ui_event_protocol.h
		src/ui/ui_event_protocol.cpp
		)


add_library(harucar ${LIB_SRC})
add_subdirectory(demo)
add_subdirectory(test)