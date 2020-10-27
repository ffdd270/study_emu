cmake_minimum_required(VERSION 3.17)
project(harucar-test)

set(CMAKE_CXX_STANDARD 17)

include_directories(
		${HARUCAR_ROOT_PATH}/include
		${HARUCAR_ROOT_PATH}/test
)


set(TEST_SRC
		main.cpp
		catch.hpp
		cpu/cpu_provider.cpp
		ui/ui_event_protocol_structure.cpp
		common/util.cpp
		common/logger.cpp)

add_executable(harucar-test ${TEST_SRC}  )
target_link_libraries( harucar-test PRIVATE harucar )