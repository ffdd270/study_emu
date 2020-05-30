#ifndef _CHIP8_DISASM_BUILD

//
// Created by ffdd270 on 2020-05-20.
//


#include "../Chip8.h"
#include <cstdlib>

// [code]
// Clear the display.
void Chip8::opCode00E0(WORD opCode)
{
	memset( &mScreenData[0][0], 0, sizeof(mScreenData));
}

// [code]
// Return from a subroutine.
//The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
void Chip8::opCode00EE(WORD opCode)
{
	mProgramCounter = mStack.back( ) ;
	mStack.pop_back( ) ;
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

// [code]
//LD (LoaD) I, addr. register I(mAddressIndex) = NNN.
//레지스터 I의 값을 NNN으로 설정한다.
void Chip8::opCodeANNN(WORD opCode)
{
	WORD NNN = ( opCode & 0x0FFF );
	mAddressIndex = NNN;
}

// [code]
//JP(JumP) V0, addr. PC = NNN + V0.
//Program Count를 NNN + V0로 점프한다.
void Chip8::opCodeBNNN(WORD opCode)
{
	WORD NNN = ( opCode & 0x0FFF );
	mProgramCounter = NNN + mRegisters[0];
}

// [code]
//RND(random) Vx, Byte(kk). Vx = random byte(255) AND kk.
//0에서 255까지의 값을 생성한 후, kk값으로 AND 연산을 한 결과를 Vx에 저장한다. AND에 관한 정보는 8xy2를 참조.
void Chip8::opCodeCXKK(WORD opCode)
{
	BYTE register_index = ( 0x0F00 & opCode ) >> 8;
	BYTE and_op_byte = 0x00FF & opCode;

	generateRandomValue();

	mRegisters[register_index] = mRandomValue & and_op_byte;
}


// [code]
// DRW Vx, Vy, nibble. 메모리 레지스터 ( mAddressIndex ) I 에서 부터 N 바이트에 읽은 스프라이트를 (x = Vx, y = Vy) 에 표시하고. Vf = Collision 설정.
// 주소 I부터 N바이트에 있는 스프라이트를 부른 다음, ( Vx, Vy ) 좌표에 그린다. 그리는 방식은 기존 화면에 XOR.
// 만약 이로 인해 픽셀이 지워지면 Vf를 1로 설정. 아니면 0으로 설정.
// 스프라이트가 디스플레이의 외부로 위치하도록 좌표를 설정하면, 반대편 화면 ( 오른쪽에 배치하면 왼쪽에서 튀어나옴 ) 에서 나온다.
// XOR 명령은 8xy3 명령을 참조.

inline int get_screen_pos(int index, int overflow_value)
{
	// index_x < 63 ? index_x : ( (index_x % 64) - 64 )
	return index < overflow_value ? index : ( ( index % overflow_value  ) );
}

void Chip8::opCodeDXYN(WORD opCode)
{
	mRegisters[0xF] = 0; // 일단 충돌이 없는 걸로.

	BYTE load_byte_size = opCode & 0x000F;

	BYTE register_x_index = ( opCode & 0x0F00 ) >> 8;
	BYTE register_y_index = ( opCode & 0x00F0 ) >> 4;

	BYTE reg_x_value = mRegisters[register_x_index];
	BYTE reg_y_value = mRegisters[register_y_index];

	for(int index = mAddressIndex; index < mAddressIndex + load_byte_size; index++ )
	{
		BYTE line = mGameMemory[index]; // 0b00000000. 0과 1로 끄고 켜짐을 표현함.
		BYTE and_byte  = 0b10000000;

		for(int and_index = 0; and_index < 8; and_index++)
		{
			BYTE value = ( line & and_byte ) >>  ( 8 - ( and_index + 1 ) );

			if (value == 1)
			{
				int bit = 0;
				// 스크린 초과하면 스크린 좌측부터 다시 시작하는 코드.
				int index_x = get_screen_pos( reg_x_value + and_index, 64 );
				int index_y = get_screen_pos( reg_y_value + ( index - mAddressIndex ), 32 );

				if ( mScreenData[index_y][index_x] == 0 )
				{
					bit = 1;
					mRegisters[0xF] = 1;
				}

				mScreenData[index_y][index_x] = bit;
			}

			and_byte = and_byte >> 1;

		}
	}
}

// [code]
// Set F, V. I = Vx 값에 해당하는 숫자의 스프라이트 주소로.
// 레지스터 I를 Vx값에 해당하는 16진수 스프라이트 위치로 설정.
void Chip8::opCodeFX29(WORD opCode)
{
	BYTE reg_x_index = ( opCode & 0x0F00 ) >> 8;
	mAddressIndex = mRegisters[reg_x_index] * 5;
}

#endif