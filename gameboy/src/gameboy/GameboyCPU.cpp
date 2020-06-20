//
// Created by nhy20 on 2020-06-07.
//

#include "GameboyCPU.h"
#include <cstring>

bool GameboyCPU::Boot()
{
	Reset();
	return true;
}

void GameboyCPU::Reset()
{
	mPC.reg_16 = 0x1000;
	mDebugInjectionCount.reg_16 = 0x1000;
	memset( mGameMemory, 0, sizeof ( mGameMemory )  );
}

void GameboyCPU::InjectionCode(BYTE injection_code)
{
	mGameMemory[ mDebugInjectionCount.reg_16 ] = injection_code;
	mDebugInjectionCount.reg_16 += 1;
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
			nextStep0x1X(opcode, second_opcode_nibble);
			break;
		case 0x2:

			break;
		case 0x3:
			break;
		case 0x4: // 모두 LD R1, R2 명령어.
		case 0x5:
		case 0x6:
			loadR1R2Instructions(opcode, first_opcode_nibble, second_opcode_nibble);
			break;
		case 0x07:

			break;
	}

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

}


void GameboyCPU::loadR1R2Instructions(BYTE opcode, BYTE first_opcode_nibble, BYTE second_opcode_nibble)
{
	//Register
	static constexpr int register_index_hash_map[4] = { 1, 2, 3, 0 };
	int register_index = first_opcode_nibble - 4; // 4~7 = 0~3

	Register & dest_register = mRegisters.array[ register_index_hash_map[ register_index ] ];
	BYTE * dest_byte = second_opcode_nibble < 8 ? &dest_register.hi : &dest_register.lo;

	if( first_opcode_nibble == 7 && second_opcode_nibble < 8 ) // ( HL )
	{
		dest_byte = &mGameMemory[ mRegisters.HL.reg_16 ];
	}
	else if ( first_opcode_nibble == 7 && second_opcode_nibble >= 8 ) // A
	{
		dest_byte = &mRegisters.AF.hi;
	}

	// Value
	int value_index = ( second_opcode_nibble % 8 ) / 2;
	Register & value_register = mRegisters.array[ register_index_hash_map[ value_index ] ];
	BYTE value_byte = ( second_opcode_nibble % 2 ) == 0 ? value_register.hi : value_register.lo;

	if( second_opcode_nibble % 8 == 6 ) // ( HL )
	{
		value_byte = mGameMemory[ mRegisters.HL.reg_16 ];
	}
	else if( second_opcode_nibble % 8 == 7 ) // A
	{
		value_byte = mRegisters.AF.hi;
	}

	*dest_byte = value_byte;
}

BYTE GameboyCPU::immediateValue()
{
	BYTE value = mGameMemory[mPC.reg_16];
	mPC.reg_16 += 1;
	return value;
}

WORD GameboyCPU::immediateValue16()
{
	return 0;
}

void GameboyCPU::load8BitToReg(BYTE &reg_8bit)
{
	reg_8bit = immediateValue();
}

