cmake_minimum_required(VERSION 3.16)
project(gameboy)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_path( GLFW_INCLUDE_DIR glfw3.h )

include_directories(${GLFW_INCLUDE_DIR})

add_executable(gameboy src/main.cpp)
target_link_libraries(gameboy PRIVATE glfw)