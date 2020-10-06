//
// Created by nhy20 on 2020-09-30.
//
// Jump Group.

#include "../GameboyCPU.h"

void GameboyCPU::jumpToWordIfCondition(BYTE op_code)
{
	WORD jump_mem_address = immediateValue16();
	BYTE flag_check_opcode = op_code >> 4u;
	BYTE not_check_opcode = op_code & 0xfu;

	// 1100이면 Z, 아니면 C.
	BYTE flag = ( flag_check_opcode == 0b1100 ) ? GetFlagZ() : GetFlagC();
	flag = ( not_check_opcode == 0b0010 ) ? !flag : flag;

	if ( flag )
	{
		mPC.reg_16 = jump_mem_address;
	}
}


void GameboyCPU::jumpToWord(BYTE op_code)
{
	mPC.reg_16 = immediateValue16();
}

void GameboyCPU::jumpToHL(BYTE op_code)
{
	mPC.reg_16 = mRegisters.HL.reg_16;
}
