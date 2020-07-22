//
// Created by nhy20 on 2020-07-17.
//

#include "../GameboyCPU.h"



//ADD A, r
// 0b10000rrr { rrr = 8bitArgument }
// A <- A + r
// = Flag =
// Z = if A is 0
// H = if bit 3 carry
// C = if bit 7 carry
// N = Reset
void GameboyCPU::addRegAFromRegister(BYTE opCode)
{
	BYTE argument = 0b00000111 & opCode;
	BYTE & register_value = m8bitArguments[ argument ].ref;

	mRegisters.AF.hi += register_value;
	resetFlags();
	setArtihmeticFlags();
}


//ADD A, n
// 0b11000110 ( 0xC6 )
// A <- A + n
// imm8
// = Flag = ( Same as ADD A, r )
void GameboyCPU::addRegAFromImm8(BYTE opCode)
{
	mRegisters.AF.hi += immediateValue();
	resetFlags();
	setArtihmeticFlags();
}

//ADD A, (HL)
// 0b10000110 ( 0x86 )
// = Flag = ( Same as ADD A, r )
void GameboyCPU::addRegAFromMemHL(BYTE opCode)
{
	mRegisters.AF.hi += mGameMemory[ mRegisters.HL.reg_16 ];
	resetFlags();
	setArtihmeticFlags();
}

