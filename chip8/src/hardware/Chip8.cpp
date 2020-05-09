//
// Created by nhy20 on 2020-05-09.
//

#include "Chip8.h"


Chip8::Chip8()
{

}

void Chip8::CPUReset()
{
	mAddressI = 0;
	mProgramCounter = 0x200; // 왜 0x200이징;
	memset(mRegisters, 0, sizeof(mRegisters));



}
