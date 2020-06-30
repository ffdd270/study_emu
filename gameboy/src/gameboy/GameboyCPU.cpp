//
// Created by nhy20 on 2020-06-07.
//

#include "GameboyCPU.h"
#include <cstring>
#include <cassert>


GameboyCPU::GameboyCPU() : m8bitArguments( 	{
													  RefRegister8bit( mRegisters.BC.hi ), // 000
													  RefRegister8bit( mRegisters.BC.lo ), // 001
													  RefRegister8bit( mRegisters.DE.hi ), // 010
													  RefRegister8bit( mRegisters.DE.lo ), // 011
													  RefRegister8bit( mRegisters.HL.hi ), // 100
													  RefRegister8bit( mRegisters.HL.lo ), // 101
													  RefRegister8bit( mRegisters.HL.lo ), // 110 .. 은 ( HL ) 예외처리 코드를 넣고 싶다 =ㅁ=..
													  RefRegister8bit( mRegisters.AF.hi ) // 111
											  } ),
						   m16bitArguments(
								   {
								   		RefRegister16Bit( mRegisters.BC.reg_16 ),
								   		RefRegister16Bit( mRegisters.DE.reg_16 ),
								   		RefRegister16Bit( mRegisters.HL.reg_16 ),
								   		RefRegister16Bit( mSP.reg_16 ),
								   }
						   		)

{
}


bool GameboyCPU::Boot()
{
	Reset();
	return true;
}

void GameboyCPU::Reset()
{
	mPC.reg_16 = 0x1000;
	mDebugInjectionCount.reg_16 = 0x1000;
	memset( mGameMemory, 0, sizeof ( mGameMemory ) );
}

void GameboyCPU::NextStep()
{
	BYTE opcode = mGameMemory[ mPC.reg_16 ];
	mPC.reg_16 += 1;

	BYTE check_pre = 0b11000000;
	BYTE pre_two_bit = (opcode & check_pre) >> 6;

	switch ( pre_two_bit )
	{
		case 0b00:
			pre0b00(opcode);
			break;
		case 0b01:
			pre0b01(opcode);
			break;
		case 0b10:
			pre0b10(opcode);
			break;
		case 0b11:
			pre0b11(opcode);
			break;
	}

}

void GameboyCPU::pre0b00(BYTE opCode)
{

	if( ( opCode & 0b00000001 ) == 0b0001 )
	{
		loadReg16toImm16(opCode);
		return;
	}


	// 0b00rrr110 -> LD r, imm8
	if( ( opCode & 0b00000110 ) == 0b110 )
	{
		loadRegtoImm8(opCode);
		return;
	}
}

void GameboyCPU::pre0b01(BYTE opCode)
{
	BYTE dest_arg =  (( opCode & 0b00111000 ) >> 3);
	BYTE origin_arg =  (opCode & 0b00000111 );

	if( dest_arg == 0b110 ) // LD (HL), r
	{
		loadMemHLtoReg( opCode );
		return;
	}

	if( origin_arg == 0b110 ) // LD r, (H:L)
	{
		loadRegtoMemHL( opCode );
		return;
	}


	//LD r, r
	loadRegtoReg( opCode );
}

void GameboyCPU::pre0b10(BYTE opCode)
{

}

void GameboyCPU::pre0b11(BYTE opCode)
{

}

// 디버거 코드들

void GameboyCPU::InjectionMemory(BYTE injection_byte)
{
	mGameMemory[ mDebugInjectionCount.reg_16 ] = injection_byte;
	mDebugInjectionCount.reg_16 += 1;
}

BYTE GameboyCPU::GetMemoryValue(unsigned int mem_index)
{
	return mGameMemory[ mem_index ];
}


void GameboyCPU::SetMemoryValue(unsigned int mem_index, BYTE value)
{
	mGameMemory[ mem_index ] = value;
}


// LD r, r'. (r'는 밑에서 y로 표기함.
// binary :01rrryyy
void GameboyCPU::loadRegtoReg(BYTE opcode)
{
	BYTE dest_reg_index = (0b00111000 & opcode) >> 3;
	BYTE origin_reg_index = (0b00000111 & opcode);

	BYTE &ref_byte_dest = m8bitArguments[dest_reg_index].ref;
	BYTE &ref_byte_origin = m8bitArguments[origin_reg_index].ref;

	ref_byte_dest = ref_byte_origin;
}

// LD (HL), r
// 0b01110rrr
void GameboyCPU::loadMemHLtoReg(BYTE opCode)
{
	BYTE origin_reg_index = (0b00000111 & opCode);

	BYTE &ref_byte_dest = m8bitArguments[origin_reg_index].ref;
	mGameMemory[ mRegisters.HL.reg_16 ] = ref_byte_dest;
}


// LD r, ( HL )
// 0b01rrr110
void GameboyCPU::loadRegtoMemHL(BYTE opCode)
{

}

// LD r, imm8
// 0b00rrr110
// 0bnnnnnnnn
void GameboyCPU::loadRegtoImm8(BYTE opCode)
{
	BYTE dest_reg_index = (0b00111000 & opCode) >> 3;
	assert(dest_reg_index != 0b110); // 110과 같으면 오류

	BYTE & ref_byte_dest = m8bitArguments[dest_reg_index].ref;
	ref_byte_dest = immediateValue();
}

void GameboyCPU::loadR1R2Instructions(BYTE opcode, BYTE first_opcode_nibble, BYTE second_opcode_nibble)
{
	loadRegtoReg( opcode );
}

BYTE GameboyCPU::immediateValue()
{
	BYTE value = mGameMemory[mPC.reg_16];
	mPC.reg_16 += 1;
	return value;
}

WORD GameboyCPU::immediateValue16()
{
	BYTE value_hi = mGameMemory[mPC.reg_16];
	BYTE value_lo = mGameMemory[mPC.reg_16 + 1];

	mPC.reg_16 += 2;

	WORD value = ( value_hi << 8 ) | value_lo;
	return value;
}

void GameboyCPU::load8BitToReg(BYTE &reg_8bit)
{
	reg_8bit = immediateValue();
}

void GameboyCPU::loadReg16toImm16(BYTE opCode)
{
	BYTE dest_reg_index = ( opCode & 0b00110000 ) >> 4;
	WORD & dest_ref_word = m16bitArguments[ dest_reg_index ].ref;

	dest_ref_word = immediateValue16();
}

