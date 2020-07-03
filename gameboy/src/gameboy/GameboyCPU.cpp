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
								   		RefRegister16Bit( mRegisters.BC.reg_16 ), //00
 								   		RefRegister16Bit( mRegisters.DE.reg_16 ), //01
								   		RefRegister16Bit( mRegisters.HL.reg_16 ), //10
								   		RefRegister16Bit( mSP.reg_16 ), // 11
								   }
						   		),
						   mFuncMap( { nullptr } )

{
	// 함수 맵 꼭 만들기
	pre0b00GenerateFuncMap();
	pre0b01GenerateFuncMap();

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
	BYTE opCode = mGameMemory[ mPC.reg_16 ];
	mPC.reg_16 += 1;

	auto& func = mFuncMap[ opCode ]; // 어떻게 배치되어있는지는 pre0b~GenerateFuncMap 함수 참고.

	if( func ==  nullptr )
	{
		assert( false );
		return;
	}

	func( this, opCode );
}

// PRE 0b00의 콜백 함수.
#define BIND_FUNC( func_name ) static void bind_##func_name\
( GameboyCPU * cpu, BYTE opCode )\
{\
	cpu->func_name\
	( opCode );\
}\

class BIND_FUNC_CLASS
{
public:

	// pre 0b01
	BIND_FUNC( loadRegToReg )
	BIND_FUNC( loadRegToMemHL )
	BIND_FUNC( loadMemHLToReg )

	// pre 0b00
	BIND_FUNC( loadRegToImm8 )
	BIND_FUNC( loadRegAToMemBC )
	BIND_FUNC( loadMemBCToRegA )
	BIND_FUNC( loadReg16toImm16 )

};



void GameboyCPU::pre0b00GenerateFuncMap()
{
	// reg8 load imm 8
	// 0b000110 ~ 0b111110
	for(BYTE i = 0b000; i <= 0b111; i++)
	{
		BYTE opCode = 0b00000000 | ( i << 3 ) | 0b110;
		mFuncMap[ opCode ] = BIND_FUNC_CLASS::bind_loadRegToImm8;
	}


	// A Load (BC)
	// 0b001010
	mFuncMap[ 0b00001010 ] = BIND_FUNC_CLASS::bind_loadRegAToMemBC;

	// (BC) Load A
	mFuncMap[ 0b00000010 ] = BIND_FUNC_CLASS::bind_loadMemBCToRegA;


	// reg16 load imm16
	for(BYTE i = 0b00; i <= 0b11; i++)
	{
		BYTE opCode = 0b00000000 | ( i << 4 ) | 0b0001;
		mFuncMap[ opCode ] = BIND_FUNC_CLASS::bind_loadReg16toImm16;
	}
}

void GameboyCPU::pre0b01GenerateFuncMap()
{
	// reg8 load reg8
	// 0b000000~0b111111
	for( BYTE i = 0b000; i <= 0b111; i++ )
	{
		for( BYTE j = 0b000; j <= 0b111; j++ )
		{
			if ( i == j  ) { continue; } // 같은 인자에 대한 연산은 없음. LD rrr, rrr 같이..

			BYTE opCode = 0b01000000 | (i << 3) | j;

			if( i == 0b110 ) // ( HL ) load reg.
			{
				mFuncMap[ opCode ] = BIND_FUNC_CLASS::bind_loadMemHLToReg;
				continue;
 			}

			if ( j == 0b110 ) // reg load ( HL )
			{
				mFuncMap[ opCode ] = BIND_FUNC_CLASS::bind_loadRegToMemHL;
				continue;
			}

			// reg load reg, 0brrryyy ( rrr, yyy != 0b110, rrr != yyy )
			mFuncMap[ opCode ] = BIND_FUNC_CLASS::bind_loadRegToReg;
		}
	}
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

// 여기서부터는 명령어들.


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


//LD (BC), A (1)
// 0b00000010 (0x02)
// (BC)<-A
void GameboyCPU::loadMemBCToRegA(BYTE opCode)
{
	BYTE & ref_mem = mGameMemory[ mRegisters.BC.reg_16 ];
	mRegisters.AF.hi = ref_mem;
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

