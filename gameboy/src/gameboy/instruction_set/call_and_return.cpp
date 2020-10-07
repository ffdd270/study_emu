//
// Created by HaruGakkaP on 2020-10-06.
//

#include "../GameboyCPU.h"

void GameboyCPU::callWord(BYTE op_code)
{
	setWORDToStack( mPC.reg_16 );
	mPC.reg_16 = immediateValue16();
}

void GameboyCPU::callIfCondition(BYTE op_code)
{
	if (getIfConditionResult(op_code))
	{
		setWORDToStack( mPC.reg_16 );
		mPC.reg_16 = immediateValue16();
	}
}

void GameboyCPU::returnInstruction(BYTE op_code)
{
	mPC.reg_16 = getWORDFromStack();
}

void GameboyCPU::returnIfCondition(BYTE op_code)
{
	if(getIfConditionResult(op_code))
	{
		returnInstruction( op_code );
	}
}

