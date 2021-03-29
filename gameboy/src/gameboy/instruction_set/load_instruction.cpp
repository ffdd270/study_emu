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
	m8bitArguments[origin_reg_index].ref = mMemoryInterface->Get( mRegisters.HL.reg_16 );
}

// LD (HL), r
// 0b01110rrr
void GameboyCPU::loadMemHLFromReg(BYTE op_code)
{
	BYTE origin_reg_index = (0b00000111u & op_code);
	mMemoryInterface->Set( mRegisters.HL.reg_16, m8bitArguments[origin_reg_index].ref );
}

// LD (HL), imm8
// 0b01110rrr
void GameboyCPU::loadMemHLFromImm8(BYTE op_code)
{
	mMemoryInterface->Set(mRegisters.HL.reg_16, immediateValue());
}


void GameboyCPU::loadMemoryImm16FromSP( BYTE op_code )
{
	mMemoryInterface->SetWord( immediateValue16(), mSP.reg_16 );
}

//LD A, (BC) (1)
// 0b00001010 (0x0A)
// A<-(BC)
void GameboyCPU::loadRegAFromMemBC(BYTE op_code)
{
	mRegisters.AF.hi = mMemoryInterface->Get( mRegisters.BC.reg_16 );
}

//LD A, (DE)
// 0b00011010 1A
// A<-(DE)
void GameboyCPU::loadRegAFromMemDE(BYTE op_code)
{
	mRegisters.AF.hi = mMemoryInterface->Get( mRegisters.DE.reg_16 );
}


//LDD A,(HL)
// 0b00111010 (0x3A) (only on Gameboy CPU. )
// A<-(HL) and HL--;
void GameboyCPU::loadRegAFromMemHLAndDecHL( BYTE op_code )
{
	mRegisters.AF.hi = mMemoryInterface->Get( mRegisters.HL.reg_16 );
	mRegisters.HL.reg_16--;
}


//LD (BC), A (1)
// 0b00000010 (0x02)
// (BC)<-A
void GameboyCPU::loadMemBCFromRegA(BYTE op_code)
{
	mMemoryInterface->Set( mRegisters.BC.reg_16, mRegisters.AF.hi );
}

//LD (DE), A (1)
// 0b00010010 (0x12)
// (DE)<-A
void GameboyCPU::loadMemDEFromRegA(BYTE op_code)
{
	mMemoryInterface->Set( mRegisters.DE.reg_16, mRegisters.AF.hi );
}

//LD (HL+), A ( or LDI HL, A ) (1)
// 0b00100010 (0x22)
// (HL+)<-A and HL<-HL + 1
void GameboyCPU::loadMemHLFromRegAAndIncHL(BYTE op_code)
{
	mMemoryInterface->Set( mRegisters.HL.reg_16, mRegisters.AF.hi );
	mRegisters.HL.reg_16++;
}

//LD A, (HL+)
// 0b00101010 (0x2A)
// A<-(HL), HL<-HL + 1
void GameboyCPU::loadRegAFromMemHLAndIncHL(BYTE op_code)
{
	mRegisters.AF.hi = mMemoryInterface->Get( mRegisters.HL.reg_16 );
	mRegisters.HL.reg_16++;
}


//LD (HL-) , A( or LDD HL, A ) (1)
// 0b00110010 (0x32)
// HL<-A and HL<-HL - 1
void GameboyCPU::loadMemHLFromRegAAndDecHL(BYTE op_code)
{
	mMemoryInterface->Set( mRegisters.HL.reg_16, mRegisters.AF.hi );
	mRegisters.HL.reg_16--;
}


//LD DD, RR (3)
//0b00dd0001
//Imm
//Imm
void GameboyCPU::loadReg16FromImm16(BYTE op_code)
{
	BYTE dest_reg_index = ( op_code & 0b00110000u ) >> 4u;
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
	BYTE argument =  ( op_code & 0b00110000u ) >> 4u;
	setWORDToStack(mRegisters.array[argument].reg_16);
}


//POP qq
// 0b11qq0001 ( qq = { BC = 00, DE = 01, HL = 10, AF = 11 } }
// qqH <- (SP + 1), qqL <- (SP)
void GameboyCPU::popReg16(BYTE op_code)
{
	BYTE argument  = ( op_code & 0b00110000u ) >> 4u;
	mRegisters.array[argument].reg_16 = getWORDFromStack();
}

void GameboyCPU::loadHalfWordMemImm8FromRegA(BYTE op_code)
{
	BYTE imm8 = immediateValue();
	mMemoryInterface->Set( 0xff00u | imm8, mRegisters.AF.hi );
}

void GameboyCPU::loadHalfWordRegAFromMemImm8(BYTE op_code)
{
	BYTE imm8 = immediateValue();
	mRegisters.AF.hi = mMemoryInterface->Get( 0xff00u | imm8 );
}

void GameboyCPU::loadMemCFromRegA(BYTE op_code)
{
	mMemoryInterface->Set( 0xff00u | mRegisters.BC.lo, mRegisters.AF.hi );
}

void GameboyCPU::loadRegAFromMemC(BYTE op_code)
{
	mRegisters.AF.hi = mMemoryInterface->Get( 0xff00u | mRegisters.BC.lo );
}

void GameboyCPU::loadMemImm16FromRegA(BYTE op_code)
{
	mMemoryInterface->Set( immediateValue16(), mRegisters.AF.hi );
}

void GameboyCPU::loadRegAFromMemImm16(BYTE op_code)
{
	mRegisters.AF.hi = mMemoryInterface->Get( immediateValue16() );
}

void GameboyCPU::loadRegHLFromSPAddSingedImm8(BYTE op_code)
{
	char imm8 = static_cast<char>( immediateValue() );
	commonAddSPInstructionFlags(imm8);
	setFlagZ( false );
	setFlagN( false );

	if( imm8 > 0  )
	{
		BYTE check_value = static_cast<BYTE>(imm8);

		setFlagH( static_cast<int>(check_value & 0x0fu) + static_cast<int>(mSP.reg_16 & 0x0fu) > 0xf );
		setFlagC( static_cast<int>(check_value & 0xffu) + static_cast<int>(mSP.reg_16 & 0xffu) > 0xff );
	}
	else
	{
		setFlagH(false );
		setFlagC(false );
	}

	mRegisters.HL.reg_16 = mSP.reg_16 + imm8;
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
