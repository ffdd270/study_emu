cmake_minimum_required(VERSION 3.15)
project(chip8)
set(CMAKE_CXX_STANDARD 17)

set( CHIP8_ROOT ${CMAKE_CURRENT_SOURCE_DIR} )
set( SRC_PATH ${CHIP8_ROOT}/src )

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(gl3w CONFIG REQUIRED)

find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h )


include_directories(${GLFW_INCLUDE_DIR})


# 디스어셈블러 빌드 옵션
# add_compile_definitions( _CHIP8_DISASM_BUILD )


set(chip8_src_files
        ${SRC_PATH}/typedef.h
        ${SRC_PATH}/hardware/Chip8.cpp
        ${SRC_PATH}/hardware/Chip8.h
        # OPCODE 명령어들.
        ${SRC_PATH}/hardware/opcodes/opcodes_if.cpp
        ${SRC_PATH}/hardware/opcodes/opcodes_code.cpp
        ${SRC_PATH}/hardware/opcodes/opcodes_load.cpp
        ${SRC_PATH}/hardware/opcodes/opcodes_math.cpp
        ${SRC_PATH}/hardware/opcodes/opcodes_bit.cpp
        ${SRC_PATH}/hardware/opcodes/opcodes_keyborad.cpp



        # 디스어셈블러 OPCODES 명령어들
        ${SRC_PATH}/hardware/disasm_opcodes/opcodes_if.cpp
        ${SRC_PATH}/hardware/disasm_opcodes/opcodes_code.cpp
        ${SRC_PATH}/hardware/disasm_opcodes/opcodes_load.cpp
        ${SRC_PATH}/hardware/disasm_opcodes/opcodes_math.cpp
        ${SRC_PATH}/hardware/disasm_opcodes/opcodes_bit.cpp
        ${SRC_PATH}/hardware/disasm_opcodes/opcodes_keyborad.cpp


        ${SRC_PATH}/hardware/disasm_opcodes/disasm_util.h)



add_subdirectory(test)
add_executable(chip8 src/main.cpp ${chip8_src_files})

target_link_libraries(chip8 PRIVATE glfw OpenGL32 unofficial::gl3w::gl3w)
