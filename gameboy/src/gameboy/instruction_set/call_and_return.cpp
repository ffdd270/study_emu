//
// Created by HaruGakkaP on 2020-10-06.
//

#include "../GameboyCPU.h"

void GameboyCPU::callWord(BYTE op_code)
{
	WORD imm = immediateValue16();
	setWORDToStack( mPC.reg_16 ); // WORD 이후 값.
	setConditionResult( true );
	mPC.reg_16 = imm;
}

void GameboyCPU::callIfCondition(BYTE op_code)
{
	if (getIfConditionResult(op_code))
	{
		callWord( op_code );
	}
	else
	{
		mPC.reg_16 += 2; // Imm 무시.
	}
}

void GameboyCPU::returnInstruction(BYTE op_code)
{
	setConditionResult( true );
	mPC.reg_16 = getWORDFromStack();
}

void GameboyCPU::returnIfCondition(BYTE op_code)
{
	if(getIfConditionResult(op_code))
	{
		returnInstruction( op_code );
	}
}

void GameboyCPU::returnFromInterrupt(BYTE op_code)
{
	mInturruptEnable = true;
	mPC.reg_16 = getWORDFromStack();
}

void GameboyCPU::restartFromParam(BYTE op_code)
{
	static const BYTE param_to_jp_pos[] = { 0x0, 0x8, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38 };
	BYTE param = static_cast<BYTE>( op_code & 0b00111000u ) >> 3u;

	setWORDToStack( mPC.reg_16 );
	mPC.reg_16 = param_to_jp_pos[ param ];
}

