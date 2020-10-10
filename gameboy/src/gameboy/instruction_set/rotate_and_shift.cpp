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

void GameboyCPU::commonRotateRightThroughCarry(BYTE & ref_value)
{
	setFlagC( ( ref_value & 0b00000001u ) == 1 );
	ref_value >>= 1u;
	ref_value = ref_value | static_cast<BYTE>( static_cast<BYTE>( GetFlagC() ) << 7u );
}

void GameboyCPU::commonRotateRight(BYTE & ref_value)
{
	BYTE flag_value = static_cast<BYTE>( GetFlagC());

	setFlagC((ref_value & 1u) == 1);

	ref_value >>= 1u;
	ref_value |= static_cast<BYTE>(flag_value << 7u);
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

	commonRotateRightThroughCarry( ref_value );

	commonRotateAndShiftFlags(ref_value);
}

void GameboyCPU::rotateRightThroughCarryRegisterA(BYTE op_code)
{
	commonRotateRightThroughCarry(  mRegisters.AF.hi );

	setFlagZ( false ); setFlagN( false );  setFlagH( false );
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

	commonRotateRight( ref_value );

	commonRotateAndShiftFlags(ref_value);
}

void GameboyCPU::rotateRightRegisterA(BYTE op_code)
{
	commonRotateRight( mRegisters.AF.hi );

	setFlagZ( false ); setFlagN( false );  setFlagH( false );
}


void GameboyCPU::shiftLeftArithmetic(BYTE op_code)
{
	BYTE &ref_value = get8BitArgumentValue((0b00000111u) & op_code);

	setFlagC(((ref_value & 0b10000000u) >> 7u) == 1);
	ref_value <<= 1u;

	commonRotateAndShiftFlags(ref_value);
}


void GameboyCPU::shiftRightArithmetic(BYTE op_code)
{
	BYTE &ref_value = get8BitArgumentValue( (0b00000111u)  & op_code );

	setFlagC( (ref_value & 1u) == 1 );
	ref_value >>= 1u;
	// Origin Value Contains in Bit7.
	ref_value |= ( ( ref_value & 0b01000000u ) << 1u );

	commonRotateAndShiftFlags(ref_value);
}

void GameboyCPU::swapLowAndHigh(BYTE op_code)
{
	BYTE &ref_value = get8BitArgumentValue((0b00000111u) & op_code);

	ref_value = ( ref_value << 4 ) | ( ref_value >> 4);

	setFlagC(false);
	setFlagN(false);
	setFlagH(false);
	setFlagZ( ref_value == 0 );
}


void GameboyCPU::shiftRightLogical(BYTE op_code)
{
	BYTE &ref_value = get8BitArgumentValue( (0b00000111u) & op_code );
	setFlagC( (ref_value & 1u) == 1 );
	ref_value >>= 1u;
	commonRotateAndShiftFlags(ref_value);
}
