//
// Created by nhy20 on 2020-09-20.
//

#include "../GameboyCPU.h"
#include <cstring>
#include <cassert>

void GameboyCPU::commonRotateAndShiftFlags( BYTE result_value)
{
	setFlagZ( result_value == 0u );
	setFlagH( false );
	setFlagN( false );
}

void GameboyCPU::commonRotateRightThroughCarry(BYTE & argument_value)
{
	setFlagC( ( argument_value & 0b00000001u ) == 1 );
	argument_value >>= 1u;
	argument_value = argument_value | static_cast<BYTE>( static_cast<BYTE>( GetFlagC() ) << 7u );
}

void GameboyCPU::commonRotateRight(BYTE & argument_value)
{
	BYTE flag_value = static_cast<BYTE>( GetFlagC());

	setFlagC((argument_value & 1u) == 1);

	argument_value >>= 1u;
	argument_value |= static_cast<BYTE>(flag_value << 7u);
}


void GameboyCPU::rotateLeftThroughCarry(BYTE op_code)
{
	BYTE param = ( 0b00000111u ) & op_code;
	BYTE argument_value = get8BitArgumentValue( param );

	setFlagC( ((argument_value & 0b10000000u) >> 7u) == 1 );
	argument_value <<= 1u;
	argument_value = argument_value | static_cast<BYTE>( GetFlagC() );

	commonRotateAndShiftFlags(argument_value);
	set8BitArgumentValue( param, argument_value );
}

void GameboyCPU::rotateLeftThroughCarryRegisterA(BYTE op_code)
{
	rotateLeftThroughCarry( 0b00000111u );
	setFlagZ( false );
}

void GameboyCPU::rotateRightThroughCarry(BYTE op_code)
{
	BYTE param = ( 0b00000111u ) & op_code;
	BYTE argument_value = get8BitArgumentValue( param );

	commonRotateRightThroughCarry( argument_value );
	commonRotateAndShiftFlags( argument_value) ;
	set8BitArgumentValue( param, argument_value );
}

void GameboyCPU::rotateRightThroughCarryRegisterA(BYTE op_code)
{
	commonRotateRightThroughCarry(  mRegisters.AF.hi );

	setFlagZ( false ); setFlagN( false );  setFlagH( false );
}


void GameboyCPU::rotateLeft(BYTE op_code)
{
	BYTE param = ( 0b00000111u ) & op_code;
	BYTE argument_value = get8BitArgumentValue( param );
	BYTE flag_value = static_cast<BYTE>(GetFlagC());

	setFlagC( ((argument_value & 0b10000000u) >> 7u) == 1 );

	argument_value <<= 1u;
	argument_value |= flag_value;

	commonRotateAndShiftFlags(argument_value);
	set8BitArgumentValue( param, argument_value );
}

void GameboyCPU::rotateLeftRegisterA(BYTE op_code)
{
	rotateLeft( 0b00000111u );
}

void GameboyCPU::rotateRight(BYTE op_code)
{
	BYTE param = ( 0b00000111u ) & op_code;
	BYTE argument_value = get8BitArgumentValue(param);

	commonRotateRight( argument_value );
	commonRotateAndShiftFlags(argument_value);

	set8BitArgumentValue( param, argument_value );
}

void GameboyCPU::rotateRightRegisterA(BYTE op_code)
{
	commonRotateRight( mRegisters.AF.hi );

	setFlagZ( false ); setFlagN( false );  setFlagH( false );
}


void GameboyCPU::shiftLeftArithmetic(BYTE op_code)
{
	BYTE param = ( 0b00000111u ) & op_code;
	BYTE argument_value = get8BitArgumentValue(param );

	setFlagC(((argument_value & 0b10000000u) >> 7u) == 1);
	argument_value <<= 1u;

	commonRotateAndShiftFlags(argument_value);
	set8BitArgumentValue( param, argument_value );
}


void GameboyCPU::shiftRightArithmetic(BYTE op_code)
{
	BYTE param = ( 0b00000111u ) & op_code;
	BYTE argument_value = get8BitArgumentValue( param );

	setFlagC( (argument_value & 1u) == 1 );
	argument_value >>= 1u;
	// Origin Value Contains in Bit7.
	argument_value |= ( ( argument_value & 0b01000000u ) << 1u );

	commonRotateAndShiftFlags(argument_value);
	set8BitArgumentValue( param, argument_value );
}

void GameboyCPU::swapLowAndHigh(BYTE op_code)
{
	BYTE param = ( 0b00000111u ) & op_code;
	BYTE argument_value = get8BitArgumentValue( param );

	argument_value = static_cast<BYTE>( argument_value << 4u ) | static_cast<BYTE>( argument_value >> 4u);

	setFlagC(false);
	setFlagN(false);
	setFlagH(false);
	setFlagZ( argument_value == 0 );
	set8BitArgumentValue( param, argument_value );
}


void GameboyCPU::shiftRightLogical(BYTE op_code)
{
	BYTE param = ( 0b00000111u ) & op_code;
	BYTE argument_value = get8BitArgumentValue( param );
	setFlagC( (argument_value & 1u) == 1 );
	argument_value >>= 1u;
	commonRotateAndShiftFlags(argument_value);
	set8BitArgumentValue( param, argument_value );
}
