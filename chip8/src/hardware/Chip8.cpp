//
// Created by nhy20 on 2020-05-09.
//

#include "Chip8.h"
#include <cstdio>

//http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
// 위 사이트 참고.

Chip8::Chip8()
{
	CPUReset();
}


void Chip8::injectionCode(WORD code)
{
	BYTE first = (code & 0xFF00) >> 8;
	BYTE second = (code & 0x00FF);

	mGameMemory[mInjectionCounter] = first;
	mGameMemory[mInjectionCounter + 1] = second;

	mInjectionCounter += 2;
}

void Chip8::reset()
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

	mProgramCounter = 0x200; // 메모리에서 코드 시작은 0x200번부터.
	mInjectionCounter = 0x200;

	memset(mRegisters, 0, sizeof(mRegisters));


	// 게임 로드 -
	/*
	FILE * game = nullptr;
	fopen_s( &game, "rom/PONG", "rb" );
	fread_s( &mGameMemory, sizeof(mGameMemory),0xfff, 1, game);
	fclose(game);
	 */
}

/*
 * OPCODE는 CPU가 실행하는 명령어. mGameMemory로 부터 명령어를 읽는다.
 * 게임 시작 코드는위에 언급한 0x200 주소부터 있고. 첫 OpCode를 얻을 수 있다.
 * CHIP-8 에서 OP Code의 길이는 2바이트이지만, 메모리는 1바이트 크기이므로 메모리의 이전 요소와 이후 요소를 결합해야한다.
 *
 * EX ) 0x200이 0xAB이고, 0x201이 0xCB이면 opcode는 0xABDD
 */

WORD Chip8::getNextOpCode()
{
	// 주석은 모두 0x200이 0xAB, 0x201이 0xCD인 걸 가정하였다. 아. PC는 0x200이고.

	WORD res = 0;
	res = mGameMemory[ mProgramCounter ]; // 0xAB로드. res = 0xAB.
	res <<= 8; // res를 8비트 시프트. 8비트 == 2^8 == 255 == 16진수 2개 점프. res = 0xAB00
	res |= mGameMemory[ mProgramCounter + 1 ];  // 1바이트 더 읽음. 0x201은 0xCD. res에 OR 연산 하면 res = 0xABCD.
	mProgramCounter += 2; // 프로그램 카운터 증가.

	return res; // opCode 반환
}

void Chip8::nextStep()
{
	WORD opCode = getNextOpCode();
	// OP 코드 해독..
	switch (DecodeOpCodeFirst(opCode)) //opCode & 0xF000
	{
		case 0x1000: // 코드 점프! 명령어 . 0x1XXX 만 이곳에 들어올 것이다. 명령어론 JP addr ( 점프 addr. )
			opCode1NNN( opCode );
			break;
		case 0x2000:
			opCode2NNN( opCode );
			break;
	    case 0x3000:
	        opCode3XKK( opCode );
	        break;
	    case 0x4000:
	        opCode4XKK( opCode );
	        break;
	    case 0x5000:
	        opCode5XY0( opCode );
	        break;
		case 0x0000: // 기타 명령어.
		{
			// 명령어 SET 중에 0x0N00. 즉, N 부분을 명령으로 사용하는 코드는 없다. 그래서 마지막만 찾으면 됨.
			//http://devernay.free.fr/hacks/chip8/C8TECH10.HTM 여기서 opcode 참고.
			switch (DecodeOpCodeForth(opCode)) // opCode & 0x000F
			{
				case 0x0000:
					break;
				case 0x000E:
					break;
			}
		}
		break;

		default: // 아직 안 하고..
			break;
	}

}

Chip8::~Chip8()
{

}


// [code]
//JP(jump) addr ( 점프 addr. ) addr = NNN.
void Chip8::opCode1NNN(WORD opCode)
{
	mProgramCounter = opCode & 0x0FFF; // 앞 자리는 해독이 끝났으니. 실제 명령어가 점프할 데이터인 뒷 바이트를 가져온다.
}


// [code]
//call addr. ( 호출 addr ). addr = NNN. stack에 현재 주소 push.
void Chip8::opCode2NNN(WORD opCode)
{
	mStack.push_back( mProgramCounter ); //stack에 현재 주소 push.
	mProgramCounter = opCode & 0x0FFF;
}

// [if]
//SE (Skip if Equal) Vx, Byte( 같으면_건너띄기 Vx Byte ).Vx =  X. Byte = KK.
void Chip8::opCode3XKK(WORD opCode)
{
    WORD index = (opCode & 0x0F00) >> 8;
    if(mRegisters[index] == (opCode & 0x00FF))
    {
        mProgramCounter+=2;
    }
}

// [if]
//SNE (Skip if not Equal) Vx, Byte (같지_않으면_건너띄기 Vx Byte )  Vx = X. Byte = KK,
void Chip8::opCode4XKK(WORD opCode)
{
    WORD index = (opCode & 0x0F00) >> 8;
    if(mRegisters[index] != (opCode & 0x00FF))
    {
        mProgramCounter += 2;
    }
}

// [if]
//SE (Skip if Equal ) Vx, Vy ( 같으면_건너띄기  Vx Vy ). Vx =  X, Vy = Y
void Chip8::opCode5XY0(WORD opCode)
{
    WORD indexVx = (opCode & 0x0F00) >> 8;
    WORD indexVy = (opCode & 0x00F0) >> 4;
    if(mRegisters[indexVx] == mRegisters[indexVy])
    {
        mProgramCounter += 2;
    }
}

// [load]
//LD (LoaD) Vx, Byte . ( 로드 Vx Byte ) Vx = X, Byte = KK
//LD는 Load의 약자다 =ㅁ=.
void Chip8::opCode6XKK(WORD opCode)
{
	BYTE register_index = opCode & 0x0F00;
	BYTE load_byte =  opCode & 0x00FF;

	mRegisters[register_index] = load_byte;
}
