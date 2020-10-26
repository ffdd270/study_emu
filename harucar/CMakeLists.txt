cmake_minimum_required(VERSION 3.17)
project(harucar)

set(CMAKE_CXX_STANDARD 20)
set(HARUCAR_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
		include
		lib/imgui
)

set( LIB_SRC
		# Base.
		include/base/viewer_interface.h
		include/base/provider_interface.h

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

		include/ui/ui_event_protocol_structure.h
		src/ui/ui_event_protocol_structure.cpp
		
		include/util/util_string.h
		
		src/common/common_logger.cpp 
		include/common/common_logger.h)


add_library(harucar ${LIB_SRC})
add_subdirectory(demo)
add_subdirectory(test)