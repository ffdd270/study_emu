//
// Created by nhy20 on 2020-06-07.
//

#include "GameboyCPU.h"
#include <cstring>


GameboyCPU::GameboyCPU() : m8bitArguments( 	{
													  RefRegister8bit( mRegisters.BC.hi ), // 000
													  RefRegister8bit( mRegisters.BC.lo ), // 001
													  RefRegister8bit( mRegisters.DE.hi ), // 010
													  RefRegister8bit( mRegisters.DE.lo ), // 011
													  RefRegister8bit( mRegisters.HL.hi ), // 100
													  RefRegister8bit( mRegisters.HL.lo ), // 101
													  RefRegister8bit( mRegisters.HL.lo ), // 110 .. 은 ( HL ) 예외처리 코드를 넣고 싶다 =ㅁ=..
													  RefRegister8bit( mRegisters.AF.hi ) // 111
											  } )
{
}


bool GameboyCPU::Boot()
{
	Reset();
	return true;
}

void GameboyCPU::Reset()
{
	mPC.reg_16 = 0x1000;
	mDebugInjectionCount.reg_16 = 0x1000;
	memset( mGameMemory, 0, sizeof ( mGameMemory ) );
}

void GameboyCPU::NextStep()
{
	BYTE opcode = mGameMemory[ mPC.reg_16 ];
	mPC.reg_16 += 1;
	BYTE first_opcode_nibble = opcode >> 4;
	BYTE second_opcode_nibble = opcode - ( first_opcode_nibble << 4 );

	switch ( first_opcode_nibble )
	{
		case 0x0:
			nextStep0x0X( opcode, second_opcode_nibble );
			break;
		case 0x1:
			nextStep0x1X( opcode, second_opcode_nibble );
			break;
		case 0x2:
			nextStep0x2X( opcode, second_opcode_nibble );
			break;
		case 0x3:
			break;
		case 0x4: // 모두 LD R1, R2 명령어.
		case 0x5:
		case 0x6:
			loadR1R2Instructions(opcode, first_opcode_nibble, second_opcode_nibble);
			break;

		case 0x7: // 7은 예외 조건이 있다.
			loadR1R2Instructions( opcode, first_opcode_nibble, second_opcode_nibble ); // 일단은 이렇게만...
			break;
	}

}

// 디버거 코드들

void GameboyCPU::InjectionMemory(BYTE injection_byte)
{
	mGameMemory[ mDebugInjectionCount.reg_16 ] = injection_byte;
	mDebugInjectionCount.reg_16 += 1;
}

BYTE GameboyCPU::GetMemoryValue(unsigned int mem_index)
{
	return mGameMemory[ mem_index ];
}


void GameboyCPU::SetMemoryValue(unsigned int mem_index, BYTE value)
{
	mGameMemory[ mem_index ] = value;
}

// 여기서부터 각 앞 자리 바이트에 대한 처리.

void GameboyCPU::nextStep0x0X(BYTE opcode, BYTE second_opcode_nibble)
{
	switch ( second_opcode_nibble )
	{
		case 0x6: // LD B,n
			load8BitToReg( mRegisters.BC.hi  );
			break;
		case 0xE: // LD C,n
			load8BitToReg( mRegisters.BC.lo  );
			break;
	}
}


void GameboyCPU::nextStep0x1X(BYTE opcode, BYTE second_opcode_nibble)
{
	switch ( second_opcode_nibble )
	{
		case 0x6: // LD D,n
			load8BitToReg( mRegisters.DE.hi  );
			break;
		case 0xE: // LD E,n
			load8BitToReg( mRegisters.DE.lo  );
			break;
	}
}


void GameboyCPU::nextStep0x2X(BYTE opcode, BYTE second_opcode_nibble)
{
	switch ( second_opcode_nibble )
	{
		case 0x1:
			mRegisters.HL.reg_16 = immediateValue16();
			break;
		case 0x6: // LD H, n
			load8BitToReg( mRegisters.HL.hi );
			break;
		case 0xE: // LD L, n
			load8BitToReg( mRegisters.HL.lo );
			break;
		default:

			break;
	}

}


// 이 구조체는 반드시 스코프 안에서 파기 되어야 합니다 . 객체간의 이동을 고려하고 만들지 않았습니다.

// LD r, r'. (r'는 밑에서 y로 표기함.
// binary :01rrryyy
void GameboyCPU::loadRegtoReg(BYTE opcode)
{
	BYTE dest_reg_index = ( 0b00111000 & opcode ) >> 3;
	BYTE origin_reg_index = ( 0b00000111 & opcode );

	BYTE & ref_byte_dest = (dest_reg_index & 0b110) == 0b110 ?  // 0b110은 (HL)
			mGameMemory[ mRegisters.HL.reg_16 ] :m8bitArguments[ dest_reg_index ].ref;

	BYTE & ref_byte_origin = (origin_reg_index & 0b110) == 0b110 ?
			mGameMemory[ mRegisters.HL.reg_16 ] : m8bitArguments[ origin_reg_index ].ref;

	ref_byte_dest = ref_byte_origin;
}

void GameboyCPU::loadR1R2Instructions(BYTE opcode, BYTE first_opcode_nibble, BYTE second_opcode_nibble)
{
	loadRegtoReg( opcode );
}

BYTE GameboyCPU::immediateValue()
{
	BYTE value = mGameMemory[mPC.reg_16];
	mPC.reg_16 += 1;
	return value;
}

WORD GameboyCPU::immediateValue16()
{
	BYTE value_hi = mGameMemory[mPC.reg_16];
	BYTE value_lo = mGameMemory[mPC.reg_16 + 1];

	mPC.reg_16 += 2;

	WORD value = ( value_hi << 8 ) | value_lo;
	return value;
}

void GameboyCPU::load8BitToReg(BYTE &reg_8bit)
{
	reg_8bit = immediateValue();
}

