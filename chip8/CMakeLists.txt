cmake_minimum_required(VERSION 3.16)
project(chip8)
set(CMAKE_CXX_STANDARD 17)

set( CHIP8_ROOT ${CMAKE_CURRENT_SOURCE_DIR} )
set( SRC_PATH ${CHIP8_ROOT}/src )

set(chip8_src_files
        ${SRC_PATH}/typedef.h
        ${SRC_PATH}/hardware/Chip8.cpp
        ${SRC_PATH}/hardware/Chip8.h )

add_subdirectory(test)
add_executable(chip8 src/main.cpp ${chip8_src_files})