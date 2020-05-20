cmake_minimum_required(VERSION 3.16)
project(chip8_test)
set(CMAKE_CXX_STANDARD 17)

include_directories( lib/
        ${SRC_PATH}/hardware/ )

set(chip8_test_src_files
        src/main.cpp
        src/chip8_opcode_test_code.cpp src/chip8_opcode_test_if.cpp)


add_executable( chip8_test
        lib/catch.hpp
        ${chip8_src_files}
        ${chip8_test_src_files})