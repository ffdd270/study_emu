#include "../GameboyCPU.h"
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
// 0b00111010 (0x3A) (only on CHIP-8)
// 0bnnnnnnnn
// 0bnnnnnnnn
// A<-(nn)
void GameboyCPU::loadRegAToMemNN(BYTE opCode)
{
	BYTE & ref_A = mRegisters.AF.hi;
	WORD imm16 = immediateValue16();

	ref_A = mGameMemory[ imm16 ];
}


//LDD A,(HL)
// 0b00111010 (0x3A) (only on Gameboy CPU. )
// A<-(HL) and HL--;
void GameboyCPU::loadRegAToMemHLAndDecHL( BYTE opCode )
{
	mRegisters.AF.hi = mGameMemory[ mRegisters.HL.reg_16 ];
	mRegisters.HL.reg_16--;
}


//LD (BC), A (1)
// 0b00000010 (0x02)
// (BC)<-A
void GameboyCPU::loadMemBCToRegA(BYTE opCode)
{
	BYTE & ref_mem = mGameMemory[ mRegisters.BC.reg_16 ];
	ref_mem = mRegisters.AF.hi;
}

//LD (DE), A (1)
// 0b00010010 (0x12)
// (DE)<-A
void GameboyCPU::loadMemDEToRegA(BYTE opCode)
{
	BYTE & ref_mem = mGameMemory[ mRegisters.DE.reg_16 ];
	ref_mem = mRegisters.AF.hi;
}

//LD (HL+), A ( or LDI HL, A ) (1)
// 0b00100010 (0x22)
// (HL+)<-A and HL<-HL + 1
void GameboyCPU::loadMemHLToRegAAndIncHL(BYTE opCode)
{
	BYTE & ref_mem = mGameMemory[ mRegisters.HL.reg_16 ];
	ref_mem = mRegisters.AF.hi;
	mRegisters.HL.reg_16++;
}

//LD A, (HL+)
// 0b00101010 (0x2A)
// A<-(HL), HL<-HL + 1
void GameboyCPU::loadRegAToMemHLAndIncHL(BYTE opCode)
{
	mRegisters.AF.hi = mGameMemory[ mRegisters.HL.reg_16 ];
	mRegisters.HL.reg_16++;
}


//LD (HL-) , A( or LDD HL, A ) (1)
// 0b00110010 (0x32)
// HL<-A and HL<-HL - 1
void GameboyCPU::loadMemHLToRegAAndDecHL(BYTE opCode)
{
	BYTE & ref_mem = mGameMemory[ mRegisters.HL.reg_16 ];
	ref_mem = mRegisters.AF.hi;
	mRegisters.HL.reg_16--;
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

//LD SP, HL
// 0b11111001 0xF9
// SP <- HL
void GameboyCPU::loadRegSPToRegHL(BYTE opCode)
{
	mSP.reg_16 = mRegisters.HL.reg_16;
}

//PUSH qq
// 0b11qq0101 ( qq = { BC = 00, DE = 01, HL = 10, AF = 11 }
// (SP - 2) <- qqLow, (SP - 1) <- qqHi, SP<-SP - 2
void GameboyCPU::pushReg16(BYTE opCode)
{
	BYTE argument =  ( opCode & 0b00110000 ) >> 4;
	WORD & ref_16 = m16bitArguments[ argument ].ref;

	mGameMemory[ mSP.reg_16 - 2 ] = ref_16 & 0x00FF  ; // low;
	mGameMemory[ mSP.reg_16 - 1 ] = (ref_16 & 0xFF00) >> 8; // high;
	
	mSP.reg_16 -= 2;
}



