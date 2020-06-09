//
// Created by nhy20 on 2020-05-09.
//

#include "Chip8.h"
#include <cstdio>
#include <ctime>
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



BYTE * Chip8::getFontSet()
{
	static BYTE bytes[] = {
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80, // F
	};

	return bytes;
}


void Chip8::CPUReset()
{
	mAddressIndex = 0;
	mRandomValue = 0;
	/*
	 * CHIP-8의 메모리 중 0x0~0x1FF은 인터프리터를 위해 사용한다.
	 * 그래서 프로그램이 로드 되는 곳은 0x200부터일 것이고, Program Counter 는 첫 명령 시작점일 0x200을 가리켜야한다.
	 */

	srand( std::time( nullptr ) );
	mProgramCounter = 0x200; // 메모리에서 코드 시작은 0x200번부터.
	mInjectionCounter = 0x200;

	mDelayTimer = 0;
	mSoundTimer = 0;

	memset(mRegisters, 0, sizeof(mRegisters));
	memset(mScreenData, 0, sizeof(mScreenData));
	memset(mGameMemory, 0, sizeof(mGameMemory));
	memset(mKeys, 0, sizeof(mKeys));

	auto * font_set = this->getFontSet();

	for( int i = 0 ; i < getFontSetLength(); i++  )
	{
		mGameMemory[i] = font_set[i];
	}

	mStack.clear();
}

void Chip8::loadRom()
{
	FILE * game = nullptr;
	fopen_s( &game, "rom/PONG", "rb" );
	fread_s( &mGameMemory[0x200], 0xfff, 0xfff, 1, game);
	fclose(game);
}


BYTE Chip8::waitInput()
{
	for( int i = 0; i < 16; i++ )
	{
		if ( mKeys[i] == 1 )
		{
			return i;
		}
	}

	return 0xFF;
}

void Chip8::addInput(BYTE input_code)
{
	mKeys[input_code] = 1;
}


void Chip8::upInput(BYTE input_code)
{
	mKeys[input_code] = 0;
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




#ifdef _CHIP8_DISASM_BUILD

#include "disasm_opcodes/disasm_util.h"

void Chip8::pushDisASMString(const std::string &code)
{
	codes.push_back( code );
}

void Chip8::createDisASMFile()
{
	int start_address = 0x200;


	FILE * disasm = nullptr;
	fopen_s( &disasm, "rom/PONG_disasm_debug.txt", "wt" );

	for(  auto & string : codes )
	{
		std::string result = "0x" + hex_to_string(start_address) + ": " + string + "\n";
		fwrite(result.c_str(), sizeof ( char ), result.length(), disasm );

		start_address += 2;
	}

	fclose(disasm);
}

#endif

void Chip8::nextStep()
{
	if( mDelayTimer > 0 )
	{
		mDelayTimer--;
	}

	WORD opCode = getNextOpCode();

	if (opCode == 0x0000)
	{
#ifdef _CHIP8_DISASM_BUILD
		mIsEOF = true;
		return;
#endif
	}

	// OP 코드 해독..
	switch (DecodeOpCodeFirst(opCode)) //opCode & 0xF000
	{
		case 0x0000:
			switch (DecodeOpCodeForth(opCode))
			{
				case 0x0000:
					opCode00E0(opCode);
					break;
				case 0x000E:
					opCode00EE(opCode);
					break;
			}
			break;
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
	    case 0x6000:
            opCode6XKK( opCode );
            break;
		case 0x7000:
			opCode7XKK( opCode );
			break;
		case 0x8000:
			nextStep0x8( opCode );
			break;
		case 0x9000:
			opCode9XY0( opCode );
			break;
		case 0xA000:
			opCodeANNN( opCode );
			break;
		case 0xB000:
			opCodeBNNN( opCode );
			break;
		case 0xC000:
			opCodeCXKK( opCode );
			break;
		case 0xD000:	
			opCodeDXYN( opCode );
			break;
		case 0xE000:
			switch (DecodeOpCodeForth( opCode ))
			{
				case 0x000E:
					opCodeEX9E( opCode );
					break;
				case 0x0001:
					opCodeEXA1( opCode );
					break;
			}
			break;
        case 0xF000:
			nextStep0xF( opCode );
            break;
		default:
			break;
	}

	//memset(mKeys, 0, sizeof(mKeys));
}

void Chip8::nextStep0x8(WORD opCode)
{
    switch (DecodeOpCodeForth(opCode)) //4번째 비트로 함수를 구별함.
    {
        case 0x0000:
            opCode8XY0(opCode);
            break;
        case 0x0001:
            opCode8XY1( opCode );
            break;
        case 0x0002:
            opCode8XY2( opCode );
            break;
        case 0x0003:
            opCode8XY3( opCode );
            break;
        case 0x0004:
            opCode8XY4( opCode );
            break;
        case 0x0005:
            opCode8XY5( opCode );
            break;
        case 0x0006:
            opCode8XY6( opCode );
            break;
        case 0x0007:
            opCode8XY7( opCode );
            break;
        case 0x000E:
            opCode8XYE( opCode );
            break;
    }
}

void Chip8::nextStep0xF(WORD opCode)
{
	switch (DecodeOpCodeForth(opCode))
	{
	    case 0x0005:
	        switch (DecodeOpCodeThird(opCode))
            {
                case 0x0060:
                    opCodeFX65( opCode );
                    break;
                case 0x0050:
                    opCodeFX55( opCode );
                    break;
                case 0x0010:
                    opCodeFX15( opCode );
                    break;
            }
            break;
	    case 0x0007:
            opCodeFX07( opCode );
            break;
	    case 0x0008:
            opCodeFX18(opCode);
            break;
		case 0x0009:
			opCodeFX29( opCode );
			break;
	    case 0x000A:
	        opCodeFX0A( opCode );
            break;
		case 0x000E:
			opCodeFX1E( opCode );
			break;
	    case 0x0003:
	        opCodeFX33( opCode );
	        break;
	}

}


void Chip8::generateRandomValue()
{
	mRandomValue = rand() % 255;
}

Chip8::~Chip8()
{

}
