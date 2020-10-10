#include "../GameboyCPU.h"
#include <cstring>
#include <cassert>


// LD r, r'. (r'는 밑에서 y로 표기함.
// binary : 0b01rrryyy
void GameboyCPU::loadRegFromReg(BYTE op_code)
{
	BYTE dest_reg_index = (0b00111000 & op_code) >> 3;
	BYTE origin_reg_index = (0b00000111 & op_code);

	BYTE &ref_byte_dest = m8bitArguments[dest_reg_index].ref;
	BYTE &ref_byte_origin = m8bitArguments[origin_reg_index].ref;

	ref_byte_dest = ref_byte_origin;
}


// LD r, imm8
// 0b00rrr110
// 0bnnnnnnnn
void GameboyCPU::loadRegFromImm8(BYTE op_code)
{
	BYTE dest_reg_index = (0b00111000 & op_code) >> 3;
	assert(dest_reg_index != 0b110); // 110과 같으면 오류

	BYTE & ref_byte_dest = m8bitArguments[dest_reg_index].ref;
	ref_byte_dest = immediateValue();
}


// LD r, ( HL )
// 0b01rrr110
void GameboyCPU::loadRegFromMemHL(BYTE op_code)
{
	assert( (op_code & 0b00000110u) == 0b110 );

	BYTE origin_reg_index = ( (op_code & 0b00111000u) >> 3u );
	assert( origin_reg_index != 0b110 );

	BYTE & ref_byte_origin = m8bitArguments[origin_reg_index].ref;
	ref_byte_origin = mGameMemory[ mRegisters.HL.reg_16 ];
}

// LD (HL), r
// 0b01110rrr
void GameboyCPU::loadMemHLFromReg(BYTE op_code)
{
	BYTE origin_reg_index = (0b00000111u & op_code);

	BYTE &ref_byte_dest = m8bitArguments[origin_reg_index].ref;
	mGameMemory[ mRegisters.HL.reg_16 ] = ref_byte_dest;
}


//LD A, (BC) (1)
// 0b00001010 (0x0A)
// A<-(BC)
void GameboyCPU::loadRegAFromMemBC(BYTE op_code)
{
	BYTE & ref_A = mRegisters.AF.hi;
	ref_A = mGameMemory[ mRegisters.BC.reg_16 ];
}

//LD A, (DE)
// 0b00011010 1A
// A<-(DE)
void GameboyCPU::loadRegAFromMemDE(BYTE op_code)
{
	BYTE & ref_A = mRegisters.AF.hi;
	ref_A = mGameMemory[ mRegisters.DE.reg_16 ];
}


//LDD A,(HL)
// 0b00111010 (0x3A) (only on Gameboy CPU. )
// A<-(HL) and HL--;
void GameboyCPU::loadRegAFromMemHLAndDecHL( BYTE op_code )
{
	mRegisters.AF.hi = mGameMemory[ mRegisters.HL.reg_16 ];
	mRegisters.HL.reg_16--;
}


//LD (BC), A (1)
// 0b00000010 (0x02)
// (BC)<-A
void GameboyCPU::loadMemBCFromRegA(BYTE op_code)
{
	BYTE & ref_mem = mGameMemory[ mRegisters.BC.reg_16 ];
	ref_mem = mRegisters.AF.hi;
}

//LD (DE), A (1)
// 0b00010010 (0x12)
// (DE)<-A
void GameboyCPU::loadMemDEFromRegA(BYTE op_code)
{
	BYTE & ref_mem = mGameMemory[ mRegisters.DE.reg_16 ];
	ref_mem = mRegisters.AF.hi;
}

//LD (HL+), A ( or LDI HL, A ) (1)
// 0b00100010 (0x22)
// (HL+)<-A and HL<-HL + 1
void GameboyCPU::loadMemHLFromRegAAndIncHL(BYTE op_code)
{
	BYTE & ref_mem = mGameMemory[ mRegisters.HL.reg_16 ];
	ref_mem = mRegisters.AF.hi;
	mRegisters.HL.reg_16++;
}

//LD A, (HL+)
// 0b00101010 (0x2A)
// A<-(HL), HL<-HL + 1
void GameboyCPU::loadRegAFromMemHLAndIncHL(BYTE op_code)
{
	mRegisters.AF.hi = mGameMemory[ mRegisters.HL.reg_16 ];
	mRegisters.HL.reg_16++;
}


//LD (HL-) , A( or LDD HL, A ) (1)
// 0b00110010 (0x32)
// HL<-A and HL<-HL - 1
void GameboyCPU::loadMemHLFromRegAAndDecHL(BYTE op_code)
{
	BYTE & ref_mem = mGameMemory[ mRegisters.HL.reg_16 ];
	ref_mem = mRegisters.AF.hi;
	mRegisters.HL.reg_16--;
}


//LD DD, RR (3)
//0b00dd0001
//Imm
//Imm
void GameboyCPU::loadReg16FromImm16(BYTE op_code)
{
	BYTE dest_reg_index = ( op_code & 0b00110000 ) >> 4;
	WORD & dest_ref_word = m16bitArguments[ dest_reg_index ].ref;

	dest_ref_word = immediateValue16();
}

//LD SP, HL
// 0b11111001 0xF9
// SP <- HL
void GameboyCPU::loadRegSPFromRegHL(BYTE op_code)
{
	mSP.reg_16 = mRegisters.HL.reg_16;
}

//PUSH qq
// 0b11qq0101 ( qq = { BC = 00, DE = 01, HL = 10, AF = 11 }
// (SP - 2) <- qqLow, (SP - 1) <- qqHi, SP<-SP - 2
void GameboyCPU::pushReg16(BYTE op_code)
{
	BYTE argument =  ( op_code & 0b00110000 ) >> 4;
	WORD & ref_16 = m16bitArguments[ argument ].ref;

	setWORDToStack( ref_16 );
}


//POP qq
// 0b11qq0001 ( qq = { BC = 00, DE = 01, HL = 10, AF = 11 } }
// qqH <- (SP + 1), qqL <- (SP)
void GameboyCPU::popReg16(BYTE op_code)
{
	BYTE argument  = ( op_code & 0b00110000 ) >> 4;
	Register & ref_register = mRegisters.array[ argument ];

	ref_register.reg_16 = getWORDFromStack();
}

void GameboyCPU::complementCarryFlag(BYTE op_code)
{
	setFlagH( GetFlagC() );
	setFlagC( !GetFlagC() );
	setFlagN( false );
}

void GameboyCPU::setCarryFlag(BYTE op_code)
{
	setFlagH( false );
	setFlagC( true );
	setFlagN( false );
}

