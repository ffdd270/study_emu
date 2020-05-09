cmake_minimum_required(VERSION 3.16)
project(chip8)
set(CMAKE_CXX_STANDARD 17)

add_executable(chip8 src/main.cpp src/typedef.h src/hardware/Chip8.cpp src/hardware/Chip8.h)