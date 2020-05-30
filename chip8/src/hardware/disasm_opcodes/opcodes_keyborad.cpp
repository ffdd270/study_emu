#ifdef _CHIP8_DISASM_BUILD

//
// Created by ffdd270 on 2020-05-20.
//
#include "../Chip8.h"


// [keyboard]
// SKP( SKip if Pressed ) Vx.
// 키보드 확인 후 키가 눌러 져있다면, PC 2 증가.
void Chip8::opCodeEX9E(WORD opCode)
{
	BYTE vx_reg_index = ( opCode & 0x0F00 ) >> 8;
	BYTE value = mRegisters[vx_reg_index];

	if ( mKeys[value] == 1 )
	{
		mProgramCounter += 2;
	}

}




// [keyboard]
// SKNP ( SKip if Not Pressed ) Vx.
// 키보드 확인 후 키가 눌러져있지 않다면, PC 2 증가.
void Chip8::opCodeEXA1(WORD opCode)
{
	BYTE vx_reg_index = ( opCode & 0x0F00 ) >> 8;
	BYTE value = mRegisters[vx_reg_index];

	if ( mKeys[value] == 0 )
	{
		mProgramCounter += 2;
	}
}

#endif