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
	if ( not_check_opcode == 0b10 )
	{
		flag = !flag;
	}

	if ( flag )
	{
		setConditionResult( true );
		mPC.reg_16 = jump_mem_address;
	}
}


void GameboyCPU::jumpToWord(BYTE op_code)
{
	setConditionResult( true );
	mPC.reg_16 = immediateValue16();
}

void GameboyCPU::jumpToHL(BYTE op_code)
{
	setConditionResult( true );
	mPC.reg_16 = mRegisters.HL.reg_16;
}

void GameboyCPU::jumpRegister(BYTE op_code)
{
	BYTE imm = immediateValue();
	mPC.reg_16 = mPC.reg_16 + static_cast<char>( imm );
}

void GameboyCPU::jumpRegisterIfCondition(BYTE op_code)
{
	// 0x20이면 Z. 아니면 C.
	bool condition = ( op_code & 0xf0u ) == 0x20u ?  GetFlagZ() : GetFlagC();

	if ( ( op_code & 0x0fu ) == 0x0u )
	{
		condition = !condition;
	}

	BYTE imm = immediateValue();

	if( condition )
	{
		mPC.reg_16 = mPC.reg_16 + static_cast<char>( imm );
	}
}
