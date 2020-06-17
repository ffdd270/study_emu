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
			nextStep0x0X(opcode, second_opcode_nibble);
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
			load8BitToReg( mBC.hi  );
			break;
		case 0xE: // LD C,n
			load8BitToReg( mBC.lo  );
			break;
	}
}


void GameboyCPU::nextStep0x1X(BYTE opcode, BYTE second_opcode_nibble)
{

}


void GameboyCPU::loadR1R2Instructions(BYTE opcode, BYTE first_opcode_nibble, BYTE second_opcode_nibble)
{

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



// 여기서부터는 명령어 셋
void GameboyCPU::load8BitToReg(BYTE & reg_8bit)
{
	reg_8bit = immediateValue();
}
