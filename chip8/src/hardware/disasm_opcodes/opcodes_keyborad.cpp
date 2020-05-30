#ifdef _CHIP8_DISASM_BUILD

//
// Created by ffdd270 on 2020-05-20.
//
#include "../Chip8.h"
#include "disasm_util.h"

// [keyboard]
// SKP( SKip if Pressed ) Vx.
// 키보드 확인 후 키가 눌러 져있다면, PC 2 증가.
void Chip8::opCodeEX9E(WORD opCode)
{
	std::string comment = "SKP V" + hex_to_string(opCode);
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}




// [keyboard]
// SKNP ( SKip if Not Pressed ) Vx.
// 키보드 확인 후 키가 눌러져있지 않다면, PC 2 증가.
void Chip8::opCodeEXA1(WORD opCode)
{
	std::string comment = "SKNP V" + hex_to_string(opCode);
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

#endif