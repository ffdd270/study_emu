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

	WORD getNextOpCode();

	void nextStep();

	/*
	 * OP Code는 4자리다. 0xFFFF 형식으로 되어 있기도 하고..
	 * 그래서 최대수의 AND 연산으로 각 자리수를 구해줄 수 있다. 이를태면..
	 * 0xABCD & 0xF000 = A. 아렇게.
	 */


	static inline WORD DecodeOpCodeFirst( WORD opcode ) { return opcode & 0xF000; }
	static inline WORD DecodeOpCodeSecond( WORD opcode )  { return opcode & 0x0F00; }
	static inline WORD DecodeOpCodeThird( WORD opcode ) { return opcode & 0x00F0; }
	static inline WORD DecodeOpCodeForth( WORD opcode ) { return opcode & 0x000F; }

private: //명령어를 실제로 수행하는 곳.

	void opCode1NNN(WORD opCode);

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
	WORD mAddressIndex; // 레지스터 I의 16비트 주소.
	WORD mProgramCounter; // 16비트 프로그램 카운터.

	// 함수 호출시 리턴 위치를 알기 위해 위치를 기록해 놓는 콜 스택.
	std::vector<WORD> mStack;
};


#endif //CHIP8_CHIP8_H
