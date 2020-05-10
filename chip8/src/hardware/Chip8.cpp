//
// Created by nhy20 on 2020-05-09.
//

#include "Chip8.h"
#include <cstdio>

Chip8::Chip8()
{
	CPUReset();
}

void Chip8::CPUReset()
{
	mAddressIndex = 0;

	/*
	 * CHIP-8의 메모리 중 0x0~0x1FF은 인터프리터를 위해 사용한다.
	 * 그래서 프로그램이 로드 되는 곳은 0x200부터일 것이고, Program Counter 는 첫 명령 시작점일 0x200을 가리켜야한다.
	 */

	mProgramCounter = 0x200; // 왜 0x200이징;
	memset(mRegisters, 0, sizeof(mRegisters));


	// 게임 로드 -
	FILE * game = nullptr;
	fopen_s( &game, "../rom/PONG", "rb" );
	fread_s( &mGameMemory, sizeof(mGameMemory),0xfff, 1, game);
	fclose(game);
}

/*
 * OPCODE는 CPU가 실행하는 명령어. mGameMemory로 부터 명령어를 읽는다.
 * 게임 시작 코드는위에 언급한 0x200 주소부터 있고. 첫 OpCode를 얻을 수 있다.
 * CHIP-8 에서 OP Code의 길이는 2바이트이지만, 메모리는 1바이트 크기이므로 메모리의 이전 요소와 이후 요소를 결합해야한다.
 *
 * EX ) 0x200이 0xAB이고, 0x201이 0xCB이면 opcode는 0xABDD
 */

WORD Chip8::GetNextOpcode()
{
	// 주석은 모두 0x200이 0xAB, 0x201이 0xCD인 걸 가정하였다. 아. PC는 0x200이고.

	WORD res = 0;
	res = mGameMemory[ mProgramCounter ]; // 0xAB로드. res = 0xAB.
	res <<= 8; // res를 8비트 시프트. 8비트 == 2^8 == 255 == 16진수 2개 점프. res = 0xAB00
	res |= mGameMemory[ mProgramCounter + 1 ];  // 1바이트 더 읽음. 0x201은 0xCD. res에 OR 연산 하면 res = 0xABCD.

	return res; // opCode 반환
}
