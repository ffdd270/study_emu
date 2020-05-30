#ifndef _CHIP8_DISASM_BUILD

//
// Created by ffdd270 on 2020-05-20.
//

#include "../Chip8.h"
#include <array>

// [load]
//LD (LoaD) Vx, Byte . ( 로드 Vx Byte ) Vx = X, Byte = KK
//LD는 Load의 약자다 =ㅁ=.
void Chip8::opCode6XKK(WORD opCode)
{
	BYTE register_index = ( opCode & 0x0F00 ) >> 8;
	BYTE load_byte =  opCode & 0x00FF;

	mRegisters[register_index] = load_byte;
}


// [load]
//LD(LoaD) Vx, Vy. ( 로드 Vx Vy )
void Chip8::opCode8XY0(WORD opCode)
{
	BYTE vx_reg_index = ( opCode & 0x0F00 ) >> 8;
	BYTE vy_reg_index = ( opCode & 0x00F0 ) >> 4;

	mRegisters[vx_reg_index] = mRegisters[vy_reg_index];
}

// [load]
// LD Vx, DT. Delay Vx = DT.
// Vx를 DT로 Set.
void Chip8::opCodeFX07(WORD opCode)
{
	BYTE vx_reg_index = ( opCode & 0x0F00 ) >> 8;

	mRegisters[vx_reg_index] = mDelayTimer;
}

// [load]
// LD Vx, Key. Set Vx = Key.
// 키 입력까지 대기 후, Vx에 키 값을 저장. 키를 누를 떄 까지 모든 행동을 중지.
void Chip8::opCodeFX0A(WORD opCode)
{
	BYTE vx_reg_index = ( opCode & 0x0F00 ) >> 8;

	BYTE input = waitInput();

	if (input == 0xFF)
	{
		mProgramCounter -= 2; //대기..
		return;
	}

	mRegisters[vx_reg_index] = input;
}


// [load]
// LD K, Vx. Delay Timer = Vx.
// DT(Delay Timer)를 Vx로.
void Chip8::opCodeFX15(WORD opCode)
{
	BYTE vx_reg_index = ( opCode & 0x0F00 )>> 8;

	mDelayTimer = mRegisters[vx_reg_index];
}

// [load]
// LD ST, Vx. Sound Timer = Vx.
// ST(Sound Timer)를 Vx로.
void Chip8::opCodeFX18(WORD opCode)
{
	BYTE vx_reg_index = ( opCode & 0x0F00 ) >> 8;

	mSoundTimer = mRegisters[vx_reg_index];
}


// [load]
// LD B, Vx. I = BCD( Vx의 100자리 수. ) I + 1 = BCD( Vx의 10자리 수.) I + 2 = BCD( Vx의 1자리 수 ),
// 메모리 위치 I, I + 1 및 I + 2에 Vx의 백~일의 자리수의 BCD 표현을 저장. ( BCD : 4비트로 10자리 수를 표현하는 기법. )
inline std::array<BYTE, 3> get_numbers( BYTE value )
{
	std::array<BYTE, 3> numbers = { 0 };

	BYTE number = value;

	for( int i = 0; i < 3 && number != 0; i++ )
	{
		numbers[i] = number % 10;
		number /= 10;
	}

	return numbers;
}


void Chip8::opCodeFX33(WORD opCode)
{
	BYTE vx_reg_index = ( opCode & 0x0F00 ) >> 8;
	BYTE vx_reg_value = mRegisters[vx_reg_index];

	std::array<BYTE, 3> bytes = get_numbers( vx_reg_value );

	mGameMemory[mAddressIndex] = bytes[2];
	mGameMemory[mAddressIndex+1] = bytes[1];
	mGameMemory[mAddressIndex+2] = bytes[0];
}

// [load]
// LD [I], Vx. Memory Address [I] to length of ( V0 to Vx ) = V0 to Vx.
// V0부터 Vx에 해당하는 값들을 메모리 주소 I에 저장 ( 그럼 메모리 끝점은 Vx 만큼 이동하겠죠? )
void Chip8::opCodeFX55(WORD opCode)
{
	BYTE reg_end_index = ( opCode & 0x0F00 ) >> 8;

	for ( BYTE i = 0; i <= reg_end_index; i++ )
	{
		mGameMemory[mAddressIndex + i] = mRegisters[i];
	}

	mAddressIndex += reg_end_index + 1;
}


// [load]
// LD Vx, [I]. V0 to Vx = Memory Address [I] to length of ( V0 to Vx ).
// 메모리 주소 I부터 I + x에 해당하는 메모리 주소까지 불러온 다음, V0부터 Vx에 저장합니다.
void Chip8::opCodeFX65(WORD opCode)
{
	BYTE reg_end_index = ( opCode & 0x0F00 ) >> 8;

	for ( BYTE i = 0; i <= reg_end_index; i++ )
	{
		mRegisters[i] = mGameMemory[mAddressIndex + i];
	}


	mAddressIndex += reg_end_index + 1;
}

#endif