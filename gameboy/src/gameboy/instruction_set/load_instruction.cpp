#include "GameboyCPU.h"
#include <cstring>
#include <cassert>


// LD r, r'. (r'는 밑에서 y로 표기함.
// binary : 0b01rrryyy
void GameboyCPU::loadRegToReg(BYTE opCode)
{
	BYTE dest_reg_index = (0b00111000 & opCode) >> 3;
	BYTE origin_reg_index = (0b00000111 & opCode);

	BYTE &ref_byte_dest = m8bitArguments[dest_reg_index].ref;
	BYTE &ref_byte_origin = m8bitArguments[origin_reg_index].ref;

	ref_byte_dest = ref_byte_origin;
}


// LD r, imm8
// 0b00rrr110
// 0bnnnnnnnn
void GameboyCPU::loadRegToImm8(BYTE opCode)
{
	BYTE dest_reg_index = (0b00111000 & opCode) >> 3;
	assert(dest_reg_index != 0b110); // 110과 같으면 오류

	BYTE & ref_byte_dest = m8bitArguments[dest_reg_index].ref;
	ref_byte_dest = immediateValue();
}


// LD r, ( HL )
// 0b01rrr110
void GameboyCPU::loadRegToMemHL(BYTE opCode)
{
	assert( (opCode & 0b00000110) == 0b110 );

	BYTE origin_reg_index = ( (opCode & 0b00111000) >> 3 );
	assert( origin_reg_index != 0b110 );

	BYTE & ref_byte_origin = m8bitArguments[origin_reg_index].ref;
	ref_byte_origin = mGameMemory[ mRegisters.HL.reg_16 ];
}

// LD (HL), r
// 0b01110rrr
void GameboyCPU::loadMemHLToReg(BYTE opCode)
{
	BYTE origin_reg_index = (0b00000111 & opCode);

	BYTE &ref_byte_dest = m8bitArguments[origin_reg_index].ref;
	mGameMemory[ mRegisters.HL.reg_16 ] = ref_byte_dest;
}


//LD A, (BC) (1)
// 0b00001010 (0x0A)
// A<-(BC)
void GameboyCPU::loadRegAToMemBC(BYTE opCode)
{
	BYTE & ref_A = mRegisters.AF.hi;
	ref_A = mGameMemory[ mRegisters.BC.reg_16 ];
}

//LD A, (DE)
// 0b00011010 1A
// A<-(DE)
void GameboyCPU::loadRegAToMemDE(BYTE opCode)
{
	BYTE & ref_A = mRegisters.AF.hi;
	ref_A = mGameMemory[ mRegisters.DE.reg_16 ];
}


//LD A, (nn)
// 0b00111010 (0x3A)
// 0bnnnnnnnn
// 0bnnnnnnnn
// A<-(nn)
void GameboyCPU::loadRegAToMemNN(BYTE opCode)
{
	BYTE & ref_A = mRegisters.AF.hi;
	WORD imm16 = immediateValue16();

	ref_A = mGameMemory[ imm16 ];
}


//LD (BC), A (1)
// 0b00000010 (0x02)
// (BC)<-A
void GameboyCPU::loadMemBCToRegA(BYTE opCode)
{
	BYTE & ref_mem = mGameMemory[ mRegisters.BC.reg_16 ];
	ref_mem = mRegisters.AF.hi;
}


//LD DD, RR (3)
//0b00dd0001
//Imm
//Imm
void GameboyCPU::loadReg16toImm16(BYTE opCode)
{
	BYTE dest_reg_index = ( opCode & 0b00110000 ) >> 4;
	WORD & dest_ref_word = m16bitArguments[ dest_reg_index ].ref;

	dest_ref_word = immediateValue16();
}