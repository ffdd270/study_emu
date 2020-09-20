//
// Created by nhy20 on 2020-09-20.
//

#include "../GameboyCPU.h"
#include <cstring>
#include <cassert>


void GameboyCPU::rotateLeftThroughCarry(BYTE op_code)
{
	BYTE param = ( 0b00000111u ) & op_code;
	BYTE & ref_value = ( param == 0b110 ) ? mGameMemory[ mRegisters.HL.reg_16 ] : m8bitArguments[ param ].ref;

	setFlagC( ((ref_value & 0b10000000u) >> 7u) == 1 );
	ref_value <<= 1u;
	ref_value = ref_value | static_cast<BYTE>( GetFlagC() );

	setFlagZ( ref_value == 0u );
	setFlagH( false );
	setFlagN( false );
}