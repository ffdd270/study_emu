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
	void CPUReset();


	// CHIP 8 내부 메모리 & CPU등 설정.

/* CHIP 8 기기는 아래로 이루어져있다.
 *
    * Memory of 0xFFF bytes ( 0xFFF 바이트짜리 메모리. )
    * 16 8-Bit data registers, called V0 to VF. VF doubles as the carry flag ( 16개의 8비트 데이터 레지스터, V0~VF라고 불린다. )
    * 16 Bit address register I which is used to access memory ( 메모리에 접근할 때 사용하는 16비트 주소 레지스터. )
    * 16 Bit program counter and a stack ( 16 비트 프로그램 카운터 ( 코드 실행 위치를 기억하는 장치 )와 스택. )
 *
 */
private:
	BYTE mGameMemory[0xFFF]; // 메모리 : 0xFFF 바이트.
	BYTE mRegisters[16]; // 16개의 레지스터들.
	WORD mAddressI; // 레지스터 I의 16비트 주소.
	WORD mProgramCounter; // 16비트 프로그램 카운터.

	std::vector<WORD> mStack; // 16비트 변수들 스택.
};


#endif //CHIP8_CHIP8_H
