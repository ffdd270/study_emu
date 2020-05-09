//
// Created by nhy20 on 2020-05-09.
//

#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

#include "../typedef.h"
#include <vector>

class Chip8
{
public:
	Chip8();

private:
	BYTE mGameMemory[0xFFF]; // 메모리 : 0xFFF 바이트.
	BYTE mRegisters[16]; // 16개의 레지스터들.
	WORD mAddressI; // 레지스터 I의 16비트 주소.
	WORD mProgramCounter; // 16비트 프로그램 카운터.

	std::vector<WORD> mStack; // 16비트 변수들 스택.
};


#endif //CHIP8_CHIP8_H
