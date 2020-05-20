//
// Created by ffdd270 on 2020-05-20.
//

#include "../Chip8.h"


// [load]
//LD (LoaD) Vx, Byte . ( 로드 Vx Byte ) Vx = X, Byte = KK
//LD는 Load의 약자다 =ㅁ=.
void Chip8::opCode6XKK(WORD opCode)
{
	BYTE register_index = opCode & 0x0F00;
	BYTE load_byte =  opCode & 0x00FF;

	mRegisters[register_index] = load_byte;
}
