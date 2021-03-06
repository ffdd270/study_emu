//
// Created by nhy20 on 2020-09-27.
//
#include "../GameboyCPU.h"


void GameboyCPU::bitTest(BYTE op_code)
{
	BYTE param = op_code & 0b00000111u;
	BYTE bit_position = ( op_code & 0b00111000u ) >> 3u;
	BYTE argument_value = get8BitArgumentValue(param);

	BYTE result = (argument_value & (0b1u << bit_position)) >> bit_position;

	setFlagZ( result == 0 );
	setFlagH( true );
	setFlagN( false );
}

void GameboyCPU::setBit(BYTE op_code)
{
	BYTE bit_position = ( op_code & 0b00111000u ) >> 3u;
	BYTE param = op_code & 0b00000111u;
	BYTE argument_value = get8BitArgumentValue(param );

	argument_value |= 0b1u << bit_position;
	set8BitArgumentValue( param, argument_value );
}

void GameboyCPU::resetBit(BYTE op_code)
{
	BYTE bit_position = ( op_code & 0b00111000u ) >> 3u;
	BYTE param = op_code & 0b00000111u;
	BYTE argument_value = get8BitArgumentValue( param );

	argument_value &= (0xffu ^ ( 0b1u << bit_position ) ); // 다르면 1, 같으면 0. 해서.. 함.
	set8BitArgumentValue( param, argument_value );
}