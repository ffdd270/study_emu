//
// Created by nhy20 on 2020-09-20.
//

#include "../GameboyCPU.h"
#include <cstring>
#include <cassert>

void GameboyCPU::commonRotateAndShiftFlags(const BYTE & result_value)
{
	setFlagZ( result_value == 0u );
	setFlagH( false );
	setFlagN( false );
}

void GameboyCPU::rotateLeftThroughCarry(BYTE op_code)
{
	BYTE & ref_value = get8BitArgumentValue( ( 0b00000111u ) & op_code );

	setFlagC( ((ref_value & 0b10000000u) >> 7u) == 1 );
	ref_value <<= 1u;
	ref_value = ref_value | static_cast<BYTE>( GetFlagC() );

	commonRotateAndShiftFlags(ref_value);
}

void GameboyCPU::rotateRightThroughCarry(BYTE op_code)
{
	BYTE & ref_value = get8BitArgumentValue( ( 0b00000111u ) & op_code );

	setFlagC( ( ref_value & 0b00000001u ) == 1 );
	ref_value >>= 1u;
	ref_value = ref_value | static_cast<BYTE>( static_cast<BYTE>( GetFlagC() ) << 7u );

	commonRotateAndShiftFlags(ref_value);
}

void GameboyCPU::rotateLeft(BYTE op_code)
{
	BYTE & ref_value = get8BitArgumentValue( ( 0b00000111u ) & op_code );
	BYTE flag_value = static_cast<BYTE>(GetFlagC());

	setFlagC( ((ref_value & 0b10000000u) >> 7u) == 1 );

	ref_value <<= 1u;
	ref_value |= flag_value;

	commonRotateAndShiftFlags(ref_value);
}

void GameboyCPU::rotateRight(BYTE op_code)
{
	BYTE &ref_value = get8BitArgumentValue((0b00000111u) & op_code);
	BYTE flag_value = static_cast<BYTE>( GetFlagC());

	setFlagC((ref_value & 1u) == 1);

	ref_value >>= 1u;
	ref_value |= static_cast<BYTE>(flag_value << 7u);

	commonRotateAndShiftFlags(ref_value);
}

void GameboyCPU::shiftLeftAccumulator(BYTE op_code)
{
	BYTE &ref_value = get8BitArgumentValue((0b00000111u) & op_code);

	setFlagC(((ref_value & 0b10000000u) >> 7u) == 1);
	ref_value <<= 1u;

	commonRotateAndShiftFlags(ref_value);
}


void GameboyCPU::shiftRightAccumulator(BYTE op_code)
{
	BYTE &ref_value = get8BitArgumentValue( (0b00000111u)  & op_code );

	setFlagC( (ref_value & 1u) == 1 );
	ref_value >>= 1u;
	// Origin Value Contains in Bit7.
	ref_value |= ( ( ref_value & 0b01000000u ) << 1u );

	commonRotateAndShiftFlags(ref_value);
}

