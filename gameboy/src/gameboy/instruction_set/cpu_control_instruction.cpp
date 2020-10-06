//
// Created by nhy20 on 2020-09-01.
//
// General-Purpose Arithmetic and CPU Control Groups

#include "../GameboyCPU.h"


// 계산 결과를 demical끼리 계산으로 변경한다.
// 입력값은 Hex가 없다.

// DAA의 특징은..
// 0x15 + 0x27 != 3C, = 42.
// 마치 정수 연산처럼 작용한다.
void GameboyCPU::decimalAdjustRegisterA( BYTE op_code )
{
	BYTE & reg_a = mRegisters.AF.hi;
	BYTE adjust_value = 0x00u;

	if ( GetFlagC() )
	{
		adjust_value |= 0x60u;
	}
	if ( GetFlagH() )
	{
		adjust_value |= 0x06u;
	}

	if( GetFlagN() )
	{
		reg_a -= adjust_value;
	}
	else
	{
		if( (reg_a & 0x0fu) > 0x09u )
		{
			adjust_value |= 0x06u;
		}

		if( reg_a > 0x99u )
		{
			adjust_value |= 0x60u;
		}

		reg_a += adjust_value;
	}

	setFlagC(  adjust_value >= 0x60 );
	setFlagH( false );
	setFlagZ( reg_a == 0 );
}

void GameboyCPU::complementRegister(BYTE op_code)
{
	mRegisters.AF.hi = ~mRegisters.AF.hi;

	setFlagH( true );
	setFlagN(  true );
}


void GameboyCPU::halt(BYTE op_code)
{
	mHalted = true;
}


void GameboyCPU::disableInterrupt(BYTE op_code)
{
	mInturruptEnable = false;
}

void GameboyCPU::enableInterrupt(BYTE op_code)
{
	mInturruptEnable = true;
}