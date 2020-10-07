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
	BYTE check_condition_param = (op_code & 0b00011000u) >> 3u;
	BYTE check_condition = (check_condition_param & 0b10u) == 0b10u ? GetFlagC() : GetFlagZ();

	// 0b01이면 Flag, 0b10이면 Not Flag.
	bool result_condition = (check_condition_param & 0b01u) == 1u ? check_condition == true : check_condition == false;

	if (result_condition)
	{
		setWORDToStack( mPC.reg_16 );
		mPC.reg_16 = immediateValue16();
	}
}

