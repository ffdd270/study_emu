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
void GameboyCPU::addRegAToRegister(BYTE opCode)
{
	BYTE argument = 0b00000111 & opCode;
	BYTE & register_value = m8bitArguments[ argument ].ref;

	mRegisters.AF.hi += register_value;
	resetFlags();

	if( mRegisters.AF.hi == 0 )
	{
		setFlagZ( true );
	}

	if( ( mRegisters.AF.hi & 0b100 ) == 0b100 )
	{
		setFlagH( true );
	}

	if ( ( mRegisters.AF.hi & 0b1000000 ) == 0b1000000 )
	{
		setFlagC( true );
	}
}



