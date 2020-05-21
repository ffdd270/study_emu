cmake_minimum_required(VERSION 3.16)
project(gameboy)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)

add_executable(gameboy src/main.cpp)

