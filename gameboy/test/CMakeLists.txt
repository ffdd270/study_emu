cmake_minimum_required(VERSION 3.15)
project(gb_test)
set(CMAKE_CXX_STANDARD 17)

include_directories( lib/
        ${SRC_PATH}/gameboy/ )

set(TEST
        src/main.cpp
        src/register.cpp
        )


add_executable( gb_test
        lib/catch.hpp
        ${TEST}
        ${GAMEBOY_SRC})