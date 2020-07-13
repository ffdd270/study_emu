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
#define BIND_FUNC( func_name ) static void func_name\
( GameboyCPU * cpu, BYTE opCode )\
{\
	cpu->func_name\
	( opCode );\
}\


// 바인더.
class BIND_FUNCS
{
public:

	// pre 0b01
	BIND_FUNC( loadRegToReg )
	BIND_FUNC( loadRegToMemHL )
	BIND_FUNC( loadMemHLToReg )

	// pre 0b00
	BIND_FUNC( loadRegToImm8 )
	BIND_FUNC( loadRegAToMemBC )
	BIND_FUNC( loadRegAToMemDE )
	// BIND_FUNC( loadRegAToMemNN )
	BIND_FUNC( loadRegAToMemHLAndIncHL )
	BIND_FUNC( loadRegAToMemHLAndDecHL )
	BIND_FUNC( loadMemBCToRegA )
	BIND_FUNC( loadReg16toImm16 )
	BIND_FUNC( loadMemDEToRegA )
	BIND_FUNC( loadMemHLToRegAAndIncHL )
	BIND_FUNC( loadMemHLToRegAAndDecHL )
};



void GameboyCPU::pre0b00GenerateFuncMap()
{
	// LD reg8, imm8 :  reg8 load imm 8
	// 0b000110 ~ 0b111110
	for(BYTE i = 0b000; i <= 0b111; i++)
	{
		BYTE opCode = 0b00000000 | ( i << 3 ) | 0b110;
		mFuncMap[ opCode ] = BIND_FUNCS::loadRegToImm8;
	}


	// LD A, (BC) : A Load (BC)
	// 0b001010
	mFuncMap[ 0b00001010 ] = BIND_FUNCS::loadRegAToMemBC;

	// LD (BC), A :  (BC) Load A
	// 0b0000010
	mFuncMap[ 0b00000010 ] = BIND_FUNCS::loadMemBCToRegA;

	// LD A, (DE)
	// 0b00011010 (0x1A)
	mFuncMap[ 0b00011010 ] = BIND_FUNCS::loadRegAToMemDE;

	// LD A, (nn) - Only on Z80
	// 0b00111010 (0x3A)
	// mFuncMap[ 0b00111010 ] = BIND_FUNCS::loadRegAToMemNN;

	//LD (DE), A (1)
	// 0b00010010 (0x12)
	// (DE)<-A
	mFuncMap[ 0b00010010 ] = BIND_FUNCS::loadMemDEToRegA;

	//LD (HL+), A ( or LDI HL, A ) (1)
	// 0b00100010 (0x22)
	// (HL+)<-A and HL<-HL + 1
	mFuncMap[ 0b00100010 ] = BIND_FUNCS::loadMemHLToRegAAndIncHL;

	//LD (HL-) , A( or LDD HL, A ) (1)
	// 0b00110010 (0x32)
	// HL<-A and HL<-HL - 1
	mFuncMap[ 0b00110010 ] = BIND_FUNCS::loadMemHLToRegAAndDecHL;

	//LD A, (HL+)
	//0b00101010 (0x2A) (only on Gameboy CPU. \
	// HL<-A and HL<-HL - 1
	mFuncMap[ 0b00101010 ] = BIND_FUNCS::loadRegAToMemHLAndIncHL;

	//LD A, (HL-)
	// 0b00111010 (0x3A) (only on Gameboy CPU. )
	// A<-(HL) and HL--;
	mFuncMap[ 0b00111010 ] = BIND_FUNCS::loadRegAToMemHLAndDecHL;


	// LD reg16, imm16 : reg16 load imm16
	// 0b00rr0001
	for(BYTE i = 0b00; i <= 0b11; i++)
	{
		BYTE opCode = 0b00000001 | ( i << 4 );
		mFuncMap[ opCode ] = BIND_FUNCS::loadReg16toImm16;
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
				mFuncMap[ opCode ] = BIND_FUNCS::loadMemHLToReg;
				continue;
 			}

			if ( j == 0b110 ) // reg load ( HL )
			{
				mFuncMap[ opCode ] = BIND_FUNCS::loadRegToMemHL;
				continue;
			}

			// reg load reg, 0brrryyy ( rrr, yyy != 0b110, rrr != yyy )
			mFuncMap[ opCode ] = BIND_FUNCS::loadRegToReg;
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



BYTE GameboyCPU::immediateValue()
{
	BYTE value = mGameMemory[mPC.reg_16];
	mPC.reg_16 += 1;
	return value;
}

WORD GameboyCPU::immediateValue16()
{
	BYTE value_lo = mGameMemory[mPC.reg_16];
	BYTE value_hi = mGameMemory[mPC.reg_16 + 1];

	mPC.reg_16 += 2;

	WORD value = ( value_hi << 8 ) | value_lo;
	return value;
}