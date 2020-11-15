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
						   mFuncMap( { nullptr } ),
						   mInturruptEnable( true ),
						   mPrefixCBFuncMap( { nullptr })
{
	// 함수 맵 꼭 만들기
	pre0b00GenerateFuncMap();
	pre0b01GenerateFuncMap();
	pre0b10GenerateFuncMap();
	pre0b11GenerateFuncMap();
	pre0xCBGenerateFuncMap();

	Reset();
}

void GameboyCPU::Reset()
{
	mPC.reg_16 = 0x1000;
	mSP.reg_16 = 0x8000;
	mDebugInjectionCount.reg_16 = 0x1000;
	memset( mGameMemory, 0, sizeof ( mGameMemory ) );

	for( Register & ref_register : mRegisters.array )
	{
		ref_register.reg_16 = 0xffff;
	}

	mHalted = false;
	mInturruptEnable = true;
	resetFlags();
}

void GameboyCPU::NextStep()
{
	if (!mHalted && ( mBreakPoints.find( mPC.reg_16 ) != mBreakPoints.end() ) )
	{
		if( mBreakPoints[ mPC.reg_16 ] ) // 그냥 Hatled.
		{
			mHalted = true;
		}
		else // Continue 요청옴.
		{
			mBreakPoints[ mPC.reg_16 ] = true;
		}

	}

	if( mHalted )
	{
		return;
	}

	BYTE op_code = mGameMemory[ mPC.reg_16 ];
	mPC.reg_16 += 1;

	if ( op_code == 0x00 ) // NOP
	{
		return;
	}

	bool isPreFixInstruction = false;

	if ( op_code == 0xCB ) // prefix
	{
		isPreFixInstruction = true;
		op_code = mGameMemory[ mPC.reg_16 ];
		mPC.reg_16 += 1;
	}

	auto& func = (isPreFixInstruction) ? mPrefixCBFuncMap[ op_code ] : mFuncMap[ op_code ]; // 어떻게 배치되어있는지는 pre0b~GenerateFuncMap 함수 참고.

	if( func ==  nullptr )
	{
		assert( false );
		return;
	}

	func( this, op_code );
}

void GameboyCPU::AddBreakPoint(WORD break_address)
{
	auto iter = mBreakPoints.find( break_address );
	if( iter != mBreakPoints.end() ) { return; }
	mBreakPoints.insert( std::make_pair( break_address, true ) );
}

void GameboyCPU::RemoveBreakPoint(WORD break_address)
{
	auto iter = mBreakPoints.find( break_address );
	if( iter == mBreakPoints.end() ) { return; }
	mBreakPoints.erase( iter );
}

void GameboyCPU::ContinueFromBreakPoint()
{
	if ( mHalted && ( mBreakPoints.find( mPC.reg_16 ) != mBreakPoints.end() ) ) // 브레이크 포인트 안이며, Halted였으면.
	{
		mBreakPoints[mPC.reg_16] = false; // Halted 유무.
		mHalted = false;
	}
}


// PRE 0b00의 콜백 함수.
#define BIND_FUNC( func_name ) static void func_name\
( GameboyCPU * cpu, BYTE op_code )\
{\
	cpu->func_name\
	( op_code );\
	cpu->addInstructionEvent( #func_name, op_code );\
}\


// 바인더.
class BIND_FUNCS
{
public:
	// pre 0b11

	//load
	BIND_FUNC( loadRegSPFromRegHL )
	BIND_FUNC( pushReg16 )
	BIND_FUNC( popReg16 )

	// arth
	BIND_FUNC( addRegAFromImm8 )
	BIND_FUNC( addRegAFromImm8AndCarry )

	BIND_FUNC( subRegAFromImm8 )
	BIND_FUNC( subRegAFromImm8AndCarry )
	BIND_FUNC( andRegAFromImm8 )
	BIND_FUNC( orRegAFromImm8 )
	BIND_FUNC( xorRegAFromImm8 )
	BIND_FUNC( cpRegAFromImm8 )

	// cpu control
	BIND_FUNC( disableInterrupt )
	BIND_FUNC( enableInterrupt )

	// jump
	BIND_FUNC( jumpToWordIfCondition )
	BIND_FUNC( jumpToWord )
	BIND_FUNC( jumpToHL )

	//call and ret
	BIND_FUNC( callWord )
	BIND_FUNC( callIfCondition )

	BIND_FUNC( returnInstruction )
	BIND_FUNC( returnIfCondition )
	BIND_FUNC( returnFromInterrupt )
	BIND_FUNC( restartFromParam )

	//Rotate Register A.
	BIND_FUNC( rotateRightThroughCarryRegisterA )
	BIND_FUNC( rotateRightRegisterA )

	// pre 0b10
	// arth
	BIND_FUNC( addRegAFromRegister )
	BIND_FUNC( addRegAFromMemHL )

	BIND_FUNC( addRegAFromRegisterAndCarry )
	BIND_FUNC( addRegAFromMemHLAndCarry )

	BIND_FUNC( subRegAFromRegister )
	BIND_FUNC( subRegAFromMemHL )

	BIND_FUNC( subRegAFromRegisterAndCarry )
	BIND_FUNC( subRegAFromMemHLAndCarry )

	BIND_FUNC( andRegAFromRegister )
	BIND_FUNC( andRegAFromMemHL )

	BIND_FUNC( orRegAFromRegister )
	BIND_FUNC( orRegAFromMemHL )

	BIND_FUNC( xorRegAFromRegister )
	BIND_FUNC( xorRegAFromMemHL )

	BIND_FUNC( cpRegAFromRegister )
	BIND_FUNC( cpRegAFromMemHL )
	// pre 0b01


	//load
	BIND_FUNC( loadRegFromReg )
	BIND_FUNC( loadRegFromMemHL )
	BIND_FUNC( loadMemHLFromReg )

	//cpu instruction
	BIND_FUNC( halt )

	//bit instruction
	BIND_FUNC( bitTest )
	BIND_FUNC( setBit )
	BIND_FUNC( resetBit )

	// pre 0b00

	//load
	BIND_FUNC( loadRegFromImm8 )
	BIND_FUNC( loadRegAFromMemBC )
	BIND_FUNC( loadRegAFromMemDE )
	BIND_FUNC( loadRegAFromMemHLAndIncHL )
	BIND_FUNC( loadRegAFromMemHLAndDecHL )
	BIND_FUNC( loadMemBCFromRegA )
	BIND_FUNC( loadReg16FromImm16 )
	BIND_FUNC( loadMemDEFromRegA )
	BIND_FUNC( loadMemHLFromRegAAndIncHL )
	BIND_FUNC( loadMemHLFromRegAAndDecHL )

	//arth
	BIND_FUNC( incRegister )
	BIND_FUNC( incMemHL )

	BIND_FUNC( decRegister )
	BIND_FUNC( decMemHL )

	BIND_FUNC( addHLFromReg16 )

	BIND_FUNC( incReg16 )
	BIND_FUNC( decReg16 )

	// cpu control
	BIND_FUNC( decimalAdjustRegisterA )
	BIND_FUNC( complementRegister )
	BIND_FUNC( complementCarryFlag )
	BIND_FUNC( setCarryFlag )

	// Prefix 0xcb
	BIND_FUNC( rotateLeftThroughCarry );
	BIND_FUNC( rotateRightThroughCarry );

	BIND_FUNC( rotateLeft );
	BIND_FUNC( rotateRight );

	BIND_FUNC( shiftLeftArithmetic );
	BIND_FUNC( shiftRightArithmetic );
	BIND_FUNC( swapLowAndHigh );
	BIND_FUNC( shiftRightLogical );
};



void GameboyCPU::pre0b00GenerateFuncMap()
{
	// LD reg8, imm8 :  reg8 load imm 8
	// 0b000110 ~ 0b111110
	for(BYTE i = 0b000; i <= 0b111; i++)
	{
		BYTE op_code = 0b00000000u | static_cast<BYTE>( i << 3u ) | 0b110u;
		mFuncMap[ op_code ] = BIND_FUNCS::loadRegFromImm8;
	}


	// LD A, (BC) : A Load (BC)
	// 0b001010
	mFuncMap[ 0b00001010 ] = BIND_FUNCS::loadRegAFromMemBC;

	// LD (BC), A :  (BC) Load A
	// 0b0000010
	mFuncMap[ 0b00000010 ] = BIND_FUNCS::loadMemBCFromRegA;

	// LD A, (DE)
	// 0b00011010 (0x1A)
	mFuncMap[ 0b00011010 ] = BIND_FUNCS::loadRegAFromMemDE;

	// LD A, (nn) - Only on Z80
	// 0b00111010 (0x3A)
	// mFuncMap[ 0b00111010 ] = BIND_FUNCS::loadRegAFromMemNN;

	//LD (DE), A (1)
	// 0b00010010 (0x12)
	// (DE)<-A
	mFuncMap[ 0b00010010 ] = BIND_FUNCS::loadMemDEFromRegA;

	//LD (HL+), A ( or LDI HL, A ) (1)
	// 0b00100010 (0x22)
	// (HL+)<-A and HL<-HL + 1
	mFuncMap[ 0b00100010 ] = BIND_FUNCS::loadMemHLFromRegAAndIncHL;

	//LD (HL-) , A( or LDD HL, A ) (1)
	// 0b00110010 (0x32)
	// HL<-A and HL<-HL - 1
	mFuncMap[ 0b00110010 ] = BIND_FUNCS::loadMemHLFromRegAAndDecHL;

	//LD A, (HL+)
	//0b00101010 (0x2A) (only on Gameboy CPU. )
	// HL<-A and HL<-HL - 1
	mFuncMap[ 0b00101010 ] = BIND_FUNCS::loadRegAFromMemHLAndIncHL;

	//LD A, (HL-)
	// 0b00111010 (0x3A) (only on Gameboy CPU. )
	// A<-(HL) and HL--;
	mFuncMap[ 0b00111010 ] = BIND_FUNCS::loadRegAFromMemHLAndDecHL;


	// LD reg16, imm16 : reg16 load imm16
	// 0b00rr0001
	for(BYTE i = 0b00; i <= 0b11; i++)
	{
		BYTE op_code = 0b00000001u | static_cast<BYTE>( i << 4u );
		mFuncMap[ op_code ] = BIND_FUNCS::loadReg16FromImm16;
	}

	// INC r
	for( BYTE i = 0b00; i <= 0b111; i++)
	{
		BYTE op_code = 0b00000100u | static_cast<BYTE>( i << 3u );

		if ( i == 0b110 )
		{
			mFuncMap[ op_code ] = BIND_FUNCS::incMemHL;
		}
		else
		{
			mFuncMap[ op_code ] = BIND_FUNCS::incRegister;
		}
	}


	// DEC Instruction
	for( BYTE i = 0b00; i <= 0b111; i++)
	{
		BYTE op_code = 0b00000101u | static_cast<BYTE>(i << 3u);

		if ( i == 0b110 )
		{
			mFuncMap[ op_code ] = BIND_FUNCS::decMemHL;
		}
		else
		{
			mFuncMap[ op_code ] = BIND_FUNCS::decRegister;
		}
	}

	// DAA Instruction
	mFuncMap[ 0x27 ] = BIND_FUNCS::decimalAdjustRegisterA;
	mFuncMap[ 0x2F ] = BIND_FUNCS::complementRegister;
	mFuncMap[ 0x3F ] = BIND_FUNCS::complementCarryFlag;
	mFuncMap[ 0x37 ] = BIND_FUNCS::setCarryFlag;

	for( uint32_t i = 0; i <= 0b11; i++ )
	{
		BYTE op_code = 0b00001001u | ( i << 4u );
		mFuncMap[ op_code ] = BIND_FUNCS::addHLFromReg16;
	}

	for ( uint32_t i = 0; i <= 0b11; i++ )
	{
		BYTE op_code = 0b00000011u | ( i << 4u );
		mFuncMap[ op_code ] = BIND_FUNCS::incReg16;
	}

	for ( uint32_t i = 0; i <= 0b11; i++ )
	{
		BYTE op_code = 0b00001011u | ( i << 4u );
		mFuncMap[ op_code ] = BIND_FUNCS::decReg16;
	}

	// ROTATE A
	mFuncMap[ 0x0F ] = BIND_FUNCS::rotateRightThroughCarryRegisterA;
	mFuncMap[ 0x1F ] = BIND_FUNCS::rotateRightRegisterA;
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

			BYTE op_code = 0b01000000u | static_cast<BYTE>(i << 3u) | j;

			if( i == 0b110 ) // ( HL ) load reg.
			{
				mFuncMap[ op_code ] = BIND_FUNCS::loadMemHLFromReg;
				continue;
 			}

			if ( j == 0b110 ) // reg load ( HL )
			{
				mFuncMap[ op_code ] = BIND_FUNCS::loadRegFromMemHL;
				continue;
			}

			// reg load reg, 0brrryyy ( rrr, yyy != 0b110, rrr != yyy )
			mFuncMap[ op_code ] = BIND_FUNCS::loadRegFromReg;
		}
	}

	mFuncMap[ 0x76 ] = BIND_FUNCS::halt;
}


void GameboyCPU::pre0b10GenerateFuncMap()
{

	for ( BYTE i = 0; i <= 0b111; i++ )
	{
		//ADD A, (HL)
		// 0b10000110 ( 0x86 )
		if ( i == 0b110 )
		{
			mFuncMap[ 0x86 ] = BIND_FUNCS::addRegAFromMemHL;
		}
		//ADD A, r
		// 0b10000rrr { rrr = 8bitArgument }
		else
		{
			BYTE op_code = 0b10000000u | i;
			mFuncMap[ op_code ] = BIND_FUNCS::addRegAFromRegister;
		}
	}


	for ( BYTE i = 0; i <= 0b111; i++ )
	{
		//ADC A, (HL)
		// 0b10001110
		if( i == 0b110 )
		{
			mFuncMap[ 0b10001110 ] = BIND_FUNCS::addRegAFromMemHLAndCarry;
		}
		//ADC A, r
		// 0b10001rrr ( rrr = 8bitArgument }
		else
		{
			BYTE op_code = 0b10001000u | i;
			mFuncMap[ op_code ] = BIND_FUNCS::addRegAFromRegisterAndCarry;
		}
	}

	for ( BYTE i = 0; i <= 0b111; i++ )
	{
		BYTE op_code = 0b10010000u | i;

		//SUB (HL)
		// 0b10010110 (0x96)
		if( i == 0b110 )
		{
			mFuncMap[ op_code ] = BIND_FUNCS::subRegAFromMemHL;
		}
		//SUB r
		// 0b10010rrr { r = m8BitArguments }
		else
		{
			mFuncMap[ op_code ] = BIND_FUNCS::subRegAFromRegister;
		}
	}

	for ( BYTE i = 0; i <= 0b111; i++ )
	{
		BYTE op_code = 0b10011000u | i;

		//SBC A, (HL)
		// 0b10011110 (0x9E)
		if ( i == 0b110 )
		{
			mFuncMap[ op_code ] = BIND_FUNCS::subRegAFromMemHLAndCarry;
		}
		//SBC A, r
		// 0b10011rrr { r = m8BitArguments }
		else
		{
			mFuncMap[ op_code ] = BIND_FUNCS::subRegAFromRegisterAndCarry;
		}
	}


	for( BYTE i = 0; i <= 0b111; i++ )
	{
		BYTE op_code = 0b10100000u | i;

		//AND (HL)
		// 0b10100110 (0xA6)
		if( i == 0b110 )
		{
			mFuncMap[ op_code ] = BIND_FUNCS::andRegAFromMemHL;
		}
		//AND r
		// 0b10100rrr
		else
		{
			mFuncMap[ op_code ] = BIND_FUNCS::andRegAFromRegister;
		}
	}

	for ( BYTE i = 0; i <= 0b111; i++ )
	{
		BYTE op_code = 0b10110000u | i;

		//OR (HL)
		// 0b10110110 ( 0xB6 )
		if( i == 0b110 )
		{
			mFuncMap[ op_code ] = BIND_FUNCS::orRegAFromMemHL;
		}
		//OR r
		// 0b10110rrr
		else
		{
			mFuncMap[ op_code ] = BIND_FUNCS::orRegAFromRegister;
		}

	}


	for ( BYTE i = 0; i <= 0b111; i++ )
	{
		BYTE op_code = 0b10101000u | i;

		//XOR (HL)
		// 0b10101110 ( 0xAE )
		if( i == 0b110 )
		{
			mFuncMap[ op_code ] = BIND_FUNCS::xorRegAFromMemHL;
		}
		//XOR r
		// 0b10101rrr { r = m8BitArguments }
		else
		{
			mFuncMap[ op_code ] = BIND_FUNCS::xorRegAFromRegister;
		}
	}

	for ( BYTE i = 0; i <= 0b111; i++ )
	{
		BYTE op_code = 0b10111000u | i;

		//CP (HL)
		// 0b10111110 ( 0xBE )
		if( i == 0b110 )
		{
			mFuncMap[ op_code ] = BIND_FUNCS::cpRegAFromMemHL;
		}
		//CP r
		// 0b10111rrr { r = m8BitArguemnts }
		else
		{
			mFuncMap[ op_code ] = BIND_FUNCS::cpRegAFromRegister;
		}
	}
}


void GameboyCPU::pre0b11GenerateFuncMap()
{
	//LD SP, HL
	// 0b11111001 0xF9
	mFuncMap[ 0b11111001 ] = BIND_FUNCS::loadRegSPFromRegHL;

	//PUSH qq
	// 0b11qq0101 ( qq = { BC = 00, DE = 01, HL = 10, AF = 11 }
	for ( BYTE i = 0; i <= 0b11; i++ )
	{
		BYTE op_code = 0b11000101u | static_cast<BYTE>( i << 4u ); // base = 0b11000101 ,i << 4 == qq.
		mFuncMap[ op_code ] = BIND_FUNCS::pushReg16;
	}

	//POP qq
	// 0b11qq0001 ( qq = { BC = 00, DE = 01, HL = 10, AF = 11 } }
	for ( BYTE i = 0; i <= 0b11; i++ ) // 굳이 PUSH와 합치지는 않았음. 3 클럭 소모 더하고 보기 좋게하는게..
	{
		BYTE op_code = 0b11000001u | static_cast<BYTE>( i << 4u ); // base = 0b11000001 ,i << 4 == qq.
		mFuncMap[ op_code ] = BIND_FUNCS::popReg16;
	}

	//ADD A, n
	// 0b11000110 ( 0xC6 )
	mFuncMap[ 0b11000110 ] = BIND_FUNCS::addRegAFromImm8;

	//ADC A, n ( Add With Carry. if Carry Set. add + 1 from result value. )
	// 0b11001110 ( 0xCE )
	mFuncMap[ 0b11001110 ] = BIND_FUNCS::addRegAFromImm8AndCarry;

	//SUB n
	// 0b11010110 (0xD6)
	mFuncMap[ 0b11010110 ] = BIND_FUNCS::subRegAFromImm8;

	//SBC A, n
	// 0b11011110 (0xDE)
	mFuncMap[ 0b11011110 ] = BIND_FUNCS::subRegAFromImm8AndCarry;

	//AND n
	// 0b11100110 (0xE6)
	mFuncMap[ 0b11100110 ] = BIND_FUNCS::andRegAFromImm8;

	//OR n
	// 0b11110110 (0xF6)
	mFuncMap[ 0b11110110 ] = BIND_FUNCS::orRegAFromImm8;

	//XOR n
	// 0b11101110 ( 0xEE )
	mFuncMap[ 0b11101110 ] = BIND_FUNCS::xorRegAFromImm8;

	//CP n
	// 0b11111110 (0xFE)
	mFuncMap[ 0b11111110 ] = BIND_FUNCS::cpRegAFromImm8;

	mFuncMap[ 0xF3 ] = BIND_FUNCS::disableInterrupt;
	mFuncMap[ 0xFB ] = BIND_FUNCS::enableInterrupt;

	//JP Groups.

	//If condition JP Groups.
	mFuncMap[ 0xDA ] = BIND_FUNCS::jumpToWordIfCondition; // JP C, WORD
	mFuncMap[ 0xD2 ] = BIND_FUNCS::jumpToWordIfCondition; // JP NC, WORD
	mFuncMap[ 0xCA ] = BIND_FUNCS::jumpToWordIfCondition; // JP Z, WORD
	mFuncMap[ 0xC2 ] = BIND_FUNCS::jumpToWordIfCondition; // JP NZ, WORD

	mFuncMap[ 0xC3 ] = BIND_FUNCS::jumpToWord;
	mFuncMap[ 0xE9 ] = BIND_FUNCS::jumpToHL;

	// CALL
	mFuncMap[ 0xCD ] = BIND_FUNCS::callWord;

	for ( BYTE i = 0b0; i <= 0b11; i++ )
	{
		mFuncMap[ 0b11000100u | static_cast<BYTE>(i << 3u) ] = BIND_FUNCS::callIfCondition; // 0b110cc100
	}

	// RET
	mFuncMap[ 0xC9 ] = BIND_FUNCS::returnInstruction;


	for ( BYTE i = 0b0; i <= 0b11; i++ )
	{
		mFuncMap[ 0b11000000u | static_cast<BYTE>(i << 3u) ] = BIND_FUNCS::returnIfCondition; // 0b110cc000
	}

	mFuncMap[ 0xD9 ] = BIND_FUNCS::returnFromInterrupt;

	for( BYTE i = 0b0; i <= 0b111; i++ )
	{
		mFuncMap[ 0b11000111u | static_cast<BYTE>( i << 3u ) ] = BIND_FUNCS::restartFromParam;
	}

}



void GameboyCPU::pre0xCBGenerateFuncMap()
{
	for ( BYTE i = 0b0; i <= 0b111; i++ )
	{
		mPrefixCBFuncMap[ i ] = BIND_FUNCS::rotateLeftThroughCarry; // 0b0 | i
	}

	for ( BYTE i = 0b0; i <= 0b111; i++ )
	{
		mPrefixCBFuncMap[ 0b1000u | i ] = BIND_FUNCS::rotateRightThroughCarry;
	}

	for ( BYTE i = 0b0; i <= 0b111; i++ )
	{
		mPrefixCBFuncMap[ 0b10000u | i ] = BIND_FUNCS::rotateLeft;
	}

	for ( BYTE i = 0b0; i <= 0b111; i++ )
	{
		mPrefixCBFuncMap[ 0b11000u | i ] = BIND_FUNCS::rotateRight;
 	}

	for ( BYTE i = 0b0; i <= 0b111; i++ )
	{
		mPrefixCBFuncMap[ 0b100000u | i ] = BIND_FUNCS::shiftLeftArithmetic;
	}

	for ( BYTE i = 0b0; i <= 0b111; i++ )
	{
		mPrefixCBFuncMap[ 0b101000u | i ] = BIND_FUNCS::shiftRightArithmetic;
	}

	for ( BYTE i = 0b0; i <= 0b111; i++ )
	{
		mPrefixCBFuncMap[ 0b110000u | i ] = BIND_FUNCS::swapLowAndHigh;
	}

	for ( BYTE i = 0b0; i <= 0b111; i++ )
	{
		mPrefixCBFuncMap[ 0b111000u | i ] = BIND_FUNCS::shiftRightLogical;
	}

	for ( BYTE bit_pos = 0b0; bit_pos <= 0b111; bit_pos++ )
	{
		for ( BYTE register_pos = 0b0; register_pos <= 0b111; register_pos++ )
		{
			BYTE base_op_code = static_cast<BYTE>(0b01u << 6u) | static_cast<BYTE>( bit_pos << 3u );
			BYTE op_code = base_op_code | register_pos;
			mPrefixCBFuncMap[ op_code ] = BIND_FUNCS::bitTest;
		}
	}

	for ( BYTE bit_pos = 0b0; bit_pos <= 0b111; bit_pos++ )
	{
		for ( BYTE register_pos = 0b0; register_pos <= 0b111; register_pos++ )
		{
			BYTE base_op_code = static_cast<BYTE>(0b11u << 6u) | static_cast<BYTE>( bit_pos << 3u );
			BYTE op_code = base_op_code | register_pos;
			mPrefixCBFuncMap[ op_code ] = BIND_FUNCS::setBit;
		}
	}

	for( BYTE bit_pos = 0b0; bit_pos <= 0b111; bit_pos++ )
	{
		for( BYTE register_pos = 0b0; register_pos <= 0b111; register_pos++ )
		{
			BYTE base_op_code = static_cast<BYTE>(0b10u << 6u) | static_cast<BYTE>( bit_pos << 3u );
			BYTE op_code = base_op_code | register_pos;
			mPrefixCBFuncMap[ op_code ] = BIND_FUNCS::resetBit;
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

GameboyMemory GameboyCPU::GetMemory()
{
	return GameboyMemory(mGameMemory, 0xffff);
}

void GameboyCPU::SetOnInstructionCallback(InstructionCallback callback)
{
	mOnInstructionCallback = std::move(callback);
}

void GameboyCPU::RemoveInstructionCallback()
{
	mOnInstructionCallback = nullptr;
}

void GameboyCPU::SetMemoryValue(unsigned int mem_index, BYTE value)
{
	mGameMemory[ mem_index ] = value;
}

void GameboyCPU::addInstructionEvent(const char *name, BYTE opcode)
{
	if ( mOnInstructionCallback != nullptr )
	{
		mOnInstructionCallback( name, opcode );
	}
}

// 여기서부터는 명령어들 Helper.

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

	WORD value = static_cast<WORD>( value_hi << 8u ) | value_lo;
	return value;
}

void GameboyCPU::setWORDToStack(WORD value)
{
	mGameMemory[ mSP.reg_16 - 1 ] = static_cast<BYTE>((value & 0xff00u) >> 8u); // HI
	mGameMemory[ mSP.reg_16 - 2 ] = static_cast<BYTE>(value & 0x00ffu); //LO

	mSP.reg_16 = mSP.reg_16 - 2;
}

WORD GameboyCPU::getWORDFromStack()
{
	BYTE hi = mGameMemory[ mSP.reg_16 + 1 ];
	BYTE lo = mGameMemory[ mSP.reg_16 ];

	mSP.reg_16 = mSP.reg_16 + 2;

	WORD word = static_cast<WORD>(hi << 8u) | lo;

	return word;
}

bool GameboyCPU::getIfConditionResult(BYTE op_code) const
{
	BYTE check_condition_param = (op_code & 0b00011000u) >> 3u;
	BYTE check_condition = (check_condition_param & 0b10u) == 0b10u ? GetFlagC() : GetFlagZ();

	// 0b00이면 Flag, 0b01이면 Not Flag.
	return (check_condition_param & 0b01u) == 1u ? check_condition == false : check_condition == true;
}

#define SET_BIT_ZERO( value, bit_pos ) value & ( 0xFFu ^ ( 0b1u << bit_pos ) )


void GameboyCPU::setFlagZ(bool flag)
{
	mRegisters.AF.lo = SET_BIT_ZERO( mRegisters.AF.lo, 7u );
	BYTE value = flag ? 0b1 : 0b0;
	mRegisters.AF.lo |= static_cast<BYTE>(value << 7u);
}

void GameboyCPU::setFlagN(bool flag)
{
	mRegisters.AF.lo = SET_BIT_ZERO( mRegisters.AF.lo, 6u );
	BYTE value = flag ? 0b1 : 0b0;
	mRegisters.AF.lo |= static_cast<BYTE>(value << 6u);
}

void GameboyCPU::setFlagH(bool flag)
{
	mRegisters.AF.lo = SET_BIT_ZERO( mRegisters.AF.lo, 5u );
	BYTE value = flag ? 0b1 : 0b0;
	mRegisters.AF.lo |= static_cast<BYTE>(value << 5u);
}

void GameboyCPU::setFlagC(bool flag)
{
	mRegisters.AF.lo = SET_BIT_ZERO( mRegisters.AF.lo, 4u );
	BYTE value = flag ? 0b1 : 0b0;
	mRegisters.AF.lo |= static_cast<BYTE>(value << 4u);
}

void GameboyCPU::resetFlags()
{
	mRegisters.AF.lo = 0;
}

BYTE &GameboyCPU::get8BitArgumentValue(BYTE param)
{
	return (param == 0b110) ? mGameMemory[mRegisters.HL.reg_16] : m8bitArguments[param].ref;
}
