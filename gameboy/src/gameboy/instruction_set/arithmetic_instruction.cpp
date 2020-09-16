//
// Created by nhy20 on 2020-07-17.
//

#include "../GameboyCPU.h"

void GameboyCPU::commonAddSetFlag(BYTE origin_value, BYTE add_value, BYTE carry)
{
	setFlagZ( static_cast<BYTE>( origin_value + add_value + carry ) == 0 );
	setFlagH( ( ( origin_value & 0x0fu ) + ( add_value & 0x0fu ) + ( carry & 0x0fu ) ) > 0x0f  );
	uint16_t result = static_cast<uint16_t>(origin_value) +
			static_cast<uint16_t>(add_value) + static_cast<uint16_t>(carry);

	setFlagC( result > 0xff ); // 올림 당함 !
	setFlagN( false );
}

// Z - > 결과값 0.
// H -> 하위 4비트 연산 결과가 음수.
// C - > 연산 결과가 음수.
// N -> Set.
void GameboyCPU::commonSubSetFlag( BYTE origin_value, BYTE sub_value, BYTE carry )
{
	setFlagZ( ( origin_value - sub_value - carry ) == 0 );
	setFlagH( ( origin_value & 0x0fu ) < ( ( sub_value & 0x0fu ) + ( carry & 0x0fu ) ) );

	uint16_t sub_value_sum = static_cast<uint16_t>(sub_value) + static_cast<uint16_t>( carry );
	setFlagC(  origin_value < sub_value_sum ); // 내림 당함!
	setFlagN( true );
}

void GameboyCPU::commonBitSetFlag()
{
	resetFlags();
	if( mRegisters.AF.hi == 0 )
	{
		setFlagZ( true );
	}
}

void GameboyCPU::commonAddRegAFromRegister(BYTE op_code, BYTE carry)
{
	BYTE argument = 0b00000111u & op_code;
	BYTE & register_value = m8bitArguments[ argument ].ref;

	resetFlags();

	commonAddSetFlag( mRegisters.AF.hi, register_value, carry);
	mRegisters.AF.hi +=  ( register_value + carry );
}

void GameboyCPU::commonAddRegAFromImm8(BYTE op_code, BYTE carry)
{
	BYTE imm_value = immediateValue();

	resetFlags();

	commonAddSetFlag( mRegisters.AF.hi, imm_value, carry);
	mRegisters.AF.hi += (imm_value + carry);
}

void GameboyCPU::commonAddRegAFromMemHL(BYTE op_code, BYTE carry)
{
	BYTE mem_value = mGameMemory[ mRegisters.HL.reg_16 ];

	resetFlags();

	commonAddSetFlag( mRegisters.AF.hi, mem_value, carry );
	mRegisters.AF.hi += ( mem_value + carry );
}


void GameboyCPU::commonSubRegAFromRegister(BYTE op_code, BYTE carry)
{
	BYTE argument = 0b00000111 & op_code;
	BYTE & register_value = m8bitArguments[ argument ].ref;

	resetFlags();

	commonSubSetFlag( mRegisters.AF.hi, register_value, carry );
	mRegisters.AF.hi -= ( register_value + carry );
}

void GameboyCPU::commonSubRegAFromImm8(BYTE op_code, BYTE carry)
{
	BYTE imm_value = immediateValue();

	commonSubSetFlag( mRegisters.AF.hi, imm_value, carry );
	mRegisters.AF.hi -= ( imm_value + carry );
}

void GameboyCPU::commonSubRegAFromMemHL(BYTE op_code, BYTE carry)
{
	BYTE mem_value = mGameMemory[ mRegisters.HL.reg_16 ];

	commonSubSetFlag( mRegisters.AF.hi, mem_value, carry );
	mRegisters.AF.hi -= ( mem_value + carry );
}



//ADD A, r
// 0b10000rrr { rrr = 8bitArgument }
// A <- A + r
// = Flag =
// Z = if A is 0
// H = if bit 3 carry
// C = if bit 7 carry
// N = Reset
void GameboyCPU::addRegAFromRegister(BYTE op_code)
{
	commonAddRegAFromRegister( op_code, 0 );
}


//ADD A, n
// 0b11000110 ( 0xC6 )
// A <- A + n
// imm8
// = Flag = ( Same as ADD A, r )
void GameboyCPU::addRegAFromImm8(BYTE op_code)
{
	commonAddRegAFromImm8( op_code, 0 );

}

//ADD A, (HL)
// 0b10000110 ( 0x86 )
// = Flag = ( Same as ADD A, r )
void GameboyCPU::addRegAFromMemHL(BYTE op_code)
{
	commonAddRegAFromMemHL(op_code, 0);
}

//ADC A, r ( Add With Carry. if Carry Set. add + 1 from result value. )
// 0b10001rrr (r = m8BitArguments)
// = Flag = ( Same as ADD A, r )
void GameboyCPU::addRegAFromRegisterAndCarry(BYTE op_code)
{
	commonAddRegAFromRegister( op_code, GetFlagC() );
}

//ADC A, n ( Add With Carry. if Carry Set. add + 1 from result value. )
// 0b11001110 ( 0xCE )
// 0bnnnnnnnn
// = Flag = ( Same as ADD A, r )
void GameboyCPU::addRegAFromImm8AndCarry(BYTE op_code)
{
	commonAddRegAFromImm8( op_code, GetFlagC() );
}

//ADC A, (HL) (  Add With Carry. if Carry Set. add + 1 from result value. )
// 0b10001110 (0x8E)
// = Flag = ( Same as ADD A, r )
void GameboyCPU::addRegAFromMemHLAndCarry(BYTE op_code)
{
	commonAddRegAFromMemHL( op_code,GetFlagC() );
}


//SUB r
// 0b10010rrr { r = m8BitArguments }
// = Flag = ( Same as ADD A, r )
void GameboyCPU::subRegAFromRegister(BYTE op_code)
{
	commonSubRegAFromRegister( op_code, 0 );
}

//SUB n
// 0b11010110 (0xD6)
// 0bnnnnnnnn
// = Flag = ( Same as ADD A, r )
void GameboyCPU::subRegAFromImm8(BYTE op_code)
{
	commonSubRegAFromImm8( op_code, 0 );
}

//SUB (HL)
// 0b10010110 (0x96)
// = Flag = ( Same as ADD A, r)
void GameboyCPU::subRegAFromMemHL(BYTE op_code)
{
	commonSubRegAFromMemHL( op_code, 0);
}

//SBC A, r
// 0b10011rrr { r = m8BitArguments }
// = Flag = ( Same as SUB r )
void GameboyCPU::subRegAFromRegisterAndCarry(BYTE op_code)
{
	commonSubRegAFromRegister( op_code, GetFlagC() );
}

//SBC A, n
// 0b11011110 (0xDE)
// = Flag = ( Same as SUB r )
void GameboyCPU::subRegAFromImm8AndCarry(BYTE op_code)
{
	commonSubRegAFromImm8( op_code, GetFlagC() );
}

//SBC A, (HL)
// 0b10011110 (0x9E)
// = Flag = ( Same as SBC A, (HL)
void GameboyCPU::subRegAFromMemHLAndCarry(BYTE op_code)
{
	commonSubRegAFromMemHL( op_code, GetFlagC() );
}

//AND r
// 0b10100rrr
// = Flag =
// Z = if 0 set 0.
// else = reset.
void GameboyCPU::andRegAFromRegister(BYTE op_code)
{
	BYTE argument = 0b00000111 & op_code;
	BYTE & register_value = m8bitArguments[ argument ].ref;

	mRegisters.AF.hi = mRegisters.AF.hi & register_value;
	commonBitSetFlag();
}

//AND n
// 0b11100110 (0xE6)
// = Flag = Same as AND r
void GameboyCPU::andRegAFromImm8(BYTE op_code)
{
	mRegisters.AF.hi = mRegisters.AF.hi & immediateValue();
	commonBitSetFlag();
}

//AND (HL)
// 0b10100110 (0xA6)
// = Flag = Same as AND (HL)
void GameboyCPU::andRegAFromMemHL(BYTE op_code)
{
	mRegisters.AF.hi = mRegisters.AF.hi & mGameMemory[ mRegisters.HL.reg_16 ];
	commonBitSetFlag();
}

//OR r
// 0b10110rrr { r = m8BitArguments }
// = Flag = Same as AND r
void GameboyCPU::orRegAFromRegister(BYTE op_code)
{
	BYTE argument = 0b00000111 & op_code;
	BYTE & register_value = m8bitArguments[ argument ].ref;

	mRegisters.AF.hi = mRegisters.AF.hi | register_value;
	commonBitSetFlag();
}

//OR n
// 0b11110110 ( 0xF6 )
// = Flag = Same as AND r
void GameboyCPU::orRegAFromImm8(BYTE op_code)
{
	mRegisters.AF.hi = mRegisters.AF.hi | immediateValue();
	commonBitSetFlag();
}

//OR (HL)
// 0b10110110 ( 0xB6 )
// = Flag = Same as AND r
void GameboyCPU::orRegAFromMemHL(BYTE op_code)
{
	mRegisters.AF.hi = mRegisters.AF.hi |  mGameMemory[ mRegisters.HL.reg_16 ];
	commonBitSetFlag();
}

//XOR r
// 0b10101rrr { r = m8BitArguments }
// = Flag =
// Z = if 0 set 0.
// else = reset.
void GameboyCPU::xorRegAFromRegister(BYTE op_code)
{
	BYTE argument = 0b00000111 & op_code;
	BYTE & register_value = m8bitArguments[ argument ].ref;

	mRegisters.AF.hi = mRegisters.AF.hi ^ register_value;
	commonBitSetFlag();
}

//XOR n
// 0b11101110 ( 0xEE )
// = Flag = Same as XOR r
void GameboyCPU::xorRegAFromImm8(BYTE op_code)
{
	mRegisters.AF.hi = mRegisters.AF.hi ^ immediateValue();
	commonBitSetFlag();
}

//XOR (HL)
// 0b10101110 ( 0xAE )
// = Flag = Same as XOR r
void GameboyCPU::xorRegAFromMemHL(BYTE op_code)
{
	mRegisters.AF.hi = mRegisters.AF.hi ^ mGameMemory[ mRegisters.HL.reg_16 ];
	commonBitSetFlag();
}

//CP r
// Compare N. Sub A - n, but result is only flag. a is not changed.
// 0b10111rrr { r = m8BitArguemnts }
// = Flag = Same as SUB r
void GameboyCPU::cpRegAFromRegister(BYTE op_code)
{
	BYTE a_reg_value = mRegisters.AF.hi;
	subRegAFromRegister( op_code );
	mRegisters.AF.hi = a_reg_value;
}

//CP n
// 0b11111110 (0xFE)
// = Flag = Same as SUB r
void GameboyCPU::cpRegAFromImm8(BYTE op_code)
{
	BYTE a_reg_value = mRegisters.AF.hi;
	subRegAFromImm8( op_code );
	mRegisters.AF.hi = a_reg_value;
}

//CP (HL)
// 0b10111110 (0xBE)
// = Flag = Same as SUB r
void GameboyCPU::cpRegAFromMemHL(BYTE op_code)
{
	BYTE a_reg_value = mRegisters.AF.hi;
	subRegAFromMemHL( op_code );
	mRegisters.AF.hi = a_reg_value;
}

void GameboyCPU::incRegister(BYTE op_code)
{
	BYTE argument = (0b00111000u & op_code) >> 3u;
	BYTE & register_value = m8bitArguments[ argument ].ref;
	commonAddSetFlag( register_value, 1, 0 );
	register_value++;
}

void GameboyCPU::incMemHL(BYTE op_code)
{
	commonAddSetFlag( mGameMemory[mRegisters.HL.reg_16], 1, 0 );
	mGameMemory[ mRegisters.HL.reg_16 ]++;
}

void GameboyCPU::decRegister(BYTE op_code)
{
	BYTE argument = (0b00111000u & op_code) >> 3u;
	BYTE & register_value = m8bitArguments[ argument ].ref;

	commonSubSetFlag( register_value, 1, 0 );
	register_value--;
}

void GameboyCPU::decMemHL(BYTE op_code)
{
	commonSubSetFlag( mGameMemory[mRegisters.HL.reg_16], 1, 0 );
	mGameMemory[ mRegisters.HL.reg_16 ]--;
}

void GameboyCPU::addHLFromReg16(BYTE op_code)
{
	BYTE argument = ( 0b00110000u & op_code ) >> 4u;
	WORD reg_16 = m16bitArguments[ argument ].ref;
	WORD & hl = mRegisters.HL.reg_16;

	setFlagH( (reg_16 & 0x0fffu) + (hl & 0xfffu) > 0x0fffu );
	setFlagC( reg_16 > 0xffffu - hl );
	setFlagN( false );

	hl += reg_16;
}

void GameboyCPU::incReg16(BYTE op_code)
{
	BYTE argument = ( 0b00110000u & op_code ) >> 4u;
	WORD & reg_16 = m16bitArguments[ argument ].ref;
	reg_16++;
}

void GameboyCPU::decReg16(BYTE op_code)
{
	BYTE argument = ( 0b00110000u & op_code ) >> 4u;
	WORD & reg_16 = m16bitArguments[ argument ].ref;
	reg_16--;
}