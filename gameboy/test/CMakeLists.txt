cmake_minimum_required(VERSION 3.15)
project(gb_test)
set(CMAKE_CXX_STANDARD 17)

include_directories( lib/
        ${SRC_PATH}/gameboy/ )

set(TEST
        src/main.cpp
        src/register.cpp
        src/load_instruction_set.cpp
        src/arithmetic_instruction_set.cpp src/util.h src/cpu_control_instruction.cpp)


add_executable( gb_test
        lib/catch.hpp
        ${TEST}
        ${GAMEBOY_SRC})
