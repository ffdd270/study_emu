//
// Created by nhy20 on 2020-06-07.
//

#include "GameboyCPU.h"
#include <cstring>
#include <cassert>
#include <string>
#include <utility>

// https://github.com/mohanson/gameboy/blob/master/src/cpu.rs 에서 따옴.
// 모든 수는 4가 나눠져 있는 상태임.

//  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
constexpr size_t OP_CYCLES[256] = {
		1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1, // 0
		0, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1, // 1
		2, 3, 2, 2, 1, 1, 2, 1, 2, 2, 2, 2, 1, 1, 2, 1, // 2
		2, 3, 2, 2, 3, 3, 3, 1, 2, 2, 2, 2, 1, 1, 2, 1, // 3
		1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 4
		1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 5
		1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 6
		2, 2, 2, 2, 2, 2, 0, 2, 1, 1, 1, 1, 1, 1, 2, 1, // 7
		1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 8
		1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 9
		1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // a
		1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // b
		2, 3, 3, 4, 3, 4, 2, 4, 2, 4, 3, 0, 3, 6, 2, 4, // c
		2, 3, 3, 0, 3, 4, 2, 4, 2, 4, 3, 0, 3, 0, 2, 4, // d
		3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4, // e
		3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4, // f
};

//  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
constexpr  size_t CB_CYCLES[256] = {
		2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 0
		2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 1
		2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 2
		2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 3
		2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, // 4
		2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, // 5
		2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, // 6
		2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, // 7
		2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 8
		2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // 9
		2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // a
		2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // b
		2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // c
		2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // d
		2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // e
		2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2, // f
};


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


class MemoryForTest : public MemoryInterface
{
public:
	MemoryForTest() : mMemory( { 0 }  )
	{
	}

	[[nodiscard]] BYTE Get(size_t mem_addr) const override
	{
		return mMemory[ mem_addr ];
	}

	void Set(size_t mem_addr, BYTE value) override
	{
		mMemory[ mem_addr ] = value;
	}

	void Reset() override
	{
		mMemory.fill( 0 );
	}

	[[nodiscard]] bool IsReportedInterrupt() const override
	{
		return false;
	}

	void ResolveInterrupt(InterruptsType resolve_interrupt_address) override
	{

	}
private:
	std::array<BYTE, 0x10000> mMemory;
};

std::shared_ptr<GameboyCPU> GameboyCPU::Create()
{
	std::shared_ptr<GameboyCPU> cpu = std::make_shared<GameboyCPU>();

	cpu->SetMemoryInterface( std::make_shared<MemoryForTest>() );

	return cpu;
}


std::shared_ptr<GameboyCPU> GameboyCPU::CreateWithMemoryInterface(std::shared_ptr<MemoryInterface> ptr_memory_interface)
{
	std::shared_ptr<GameboyCPU> cpu = std::make_shared<GameboyCPU>();
	cpu->SetMemoryInterface( std::move(ptr_memory_interface) );
	return cpu;
}

void GameboyCPU::SetMemoryInterface(std::shared_ptr<MemoryInterface> memory_interface_ptr)
{
	mMemoryInterface = std::move( memory_interface_ptr );
}

void GameboyCPU::Reset()
{
	mRegisters.AF.reg_16 = 0x01;
	mRegisters.AF.lo = 0xB0;
	mRegisters.BC.reg_16 = 0x0013;
	mRegisters.DE.reg_16 = 0x00D8;
	mRegisters.HL.reg_16 = 0x14D;
	mPC.reg_16 = 0x0100;
	mSP.reg_16 = 0xfffe;
	mDebugInjectionCount.reg_16 = 0x0100;

	if (mMemoryInterface != nullptr )
	{
		mMemoryInterface->Reset();
	}
	mHalted = false;
	mInturruptEnable = true;
}

void GameboyCPU::TestReset()
{
	mRegisters.AF.reg_16 = 0x00;
	mRegisters.AF.lo = 0x00;
	mRegisters.BC.reg_16 = 0x0000;
	mRegisters.DE.reg_16 = 0x0000;
	mRegisters.HL.reg_16 = 0x0000;
}

size_t GameboyCPU::NextStep()
{
	size_t clock = procInterrupt();

	if ( clock == 0 ) // 처리하지 않았음
 	{
		clock = execute();
	}

	return clock;
}


const char * GameboyCPU::TestOpCode(BYTE op_code, bool prefix)
{
	auto& func = (prefix) ? mPrefixCBFuncMap[ op_code ] : mFuncMap[ op_code ]; // 어떻게 배치되어있는지는 pre0b~GenerateFuncMap 함수 참고.
	if( func == nullptr )
	{
		throw std::logic_error("Not Impl Instruction Set, " + std::to_string( op_code ) );
	}

	return func( this, op_code, false );
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


constexpr std::array<size_t, 5> INTERRUPT_VECTORS = {
		0x40u, // V-BLANK
		0x48u, // LCD STAT
		0x50u, // TIMER
		0x58u, // SERIAL
		0x60u, // JOYPAD
};


// 사용할 인터럽트
BYTE get_use_interrupt( BYTE interrupt_vectors )
{
	for ( size_t i = 0; i < 5; i++ )
	{
		if ( ( ( interrupt_vectors & ( 0b1u << i ) ) >> i ) == 1 )
		{
			return i;
		}
	}

	return 0xff;
}

size_t GameboyCPU::procInterrupt()
{
	if(!mHalted && !mInturruptEnable) // 인터럽트 미허용 && 정지 상태면 무시.
	{
		return 0;
	}

	// 인터럽트는 5비트까지만 씀.
	BYTE interrupt_enable = mMemoryInterface->Get( 0xffff );
	BYTE interrupt_flags = mMemoryInterface->Get( 0xff0f );

	// Enable 과 Flag 둘 다 올라와야 한다.
	BYTE interrupt_vectors = interrupt_enable & interrupt_flags;
	if ( interrupt_vectors == 0x00 ) // 인터럽트 플래그가 안 올라갔슴.
	{
		return 0;
	}

	BYTE interrupt = get_use_interrupt( interrupt_vectors );
	mHalted = false; // 인터럽트가 올라갔기 때문에 중지 상태에서 벗어남.

	if ( interrupt == 0xff ) // 실행할 게 없음
	{
		return 0;
	}

	if (!mInturruptEnable) // 인터럽트 활성화가 아니면 플래그는 내리지 않는다.
	{
		return 0;
	}

	mInturruptEnable = false;
	setWORDToStack( mPC.reg_16 );  // 스택에 올리고
	mPC.reg_16 = INTERRUPT_VECTORS[ interrupt ];

	interrupt_flags = interrupt_flags & static_cast<BYTE>( ~static_cast<BYTE>( 1u << interrupt ) );
	mMemoryInterface->Set( 0xff0f, interrupt_flags );

	return 4; // 4 클락.
}

size_t GameboyCPU::CheckAdditionalClock( BYTE opcode ) const
{
	switch (opcode)
	{
		case 0x20: //JR NZ, r8 12/8 → 0x20
			if (!GetFlagZ()) { return 1; }
			break;
		case 0x28: //JR Z, r8 12/8  → 0x28
			if (GetFlagZ()) { return 1; }
			break;
		case 0x30: // JR NC, r8 12/8  → 0x30
			if (!GetFlagC()) { return 1; }
			break;
		case 0x38: // JR C, r8 12/8  → 0x38
			if (GetFlagC()) { return 1; }
			break;
		case 0xC0: //RET NZ 20/8  → 0xC0
			if (!GetFlagZ()) { return 3; }
			break;
		case 0xD0: //RET NC 20/8  → 0xD0
			if (!GetFlagC()) { return 3; }
			break;
		case 0xC2: //JP NZ, a16 16/12 →0xC2
			if (!GetFlagZ()) { return 1; }
			break;
		case 0xD2: //JP NC, a16 16/12 → 0xD2
			if (!GetFlagC()) { return 1; }
			break;
		case 0xC4: //CALL NZ, a16 24/12 → 0xC4
			if (!GetFlagZ()) { return 3; }
			break;
		case 0xD4: //CALL NC, a16 24/12 →0xD4
			if (!GetFlagC()) { return 3; }
			break;
		case 0xC8: //RET Z 20/8 → 0xC8
			if (GetFlagZ()) { return 3; }
			break;
		case 0xD8: //RET C 20/8 → 0xD8
			if (GetFlagC()) { return 3; }
			break;
		case 0xCA: //JP Z, a16 16/12  → 0xCA
			if (GetFlagZ()) { return 1; }
			break;
		case 0xDA: //JP C, a16 16/12 → 0xDA
			if (GetFlagC()) { return 1; }
			break;
		case 0xCC: //CALL Z,a16 24/12 → 0xCC
			if (GetFlagZ()) { return 3; }
			break;
		case 0xDC: //CALL C,a16 24/12 → 0xDC
			if (GetFlagC()) { return 3; }
			break;
		default:
			break;
	}
	return 0;
}


size_t GameboyCPU::execute()
{
	constexpr size_t NOP_INSTRUCTION =  OP_CYCLES[0] * 4;

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
		return NOP_INSTRUCTION;
	}

	BYTE op_code = mMemoryInterface->Get( mPC.reg_16 );
	mPC.reg_16 += 1;

	if ( op_code == 0x00 || op_code == 0x10 ) // NOP, STOP.
	{
		return NOP_INSTRUCTION;
	}

	bool isPreFixInstruction = false;

	size_t cycles = 0;

	if ( op_code == 0xCB ) // prefix
	{
		isPreFixInstruction = true;
		op_code = mMemoryInterface->Get( mPC.reg_16 );
		mPC.reg_16 += 1;
		cycles = CB_CYCLES[op_code];
	}
	else
	{
		cycles = OP_CYCLES[op_code];
	}

	auto& func = (isPreFixInstruction) ? mPrefixCBFuncMap[ op_code ] : mFuncMap[ op_code ]; // 어떻게 배치되어있는지는 pre0b~GenerateFuncMap 함수 참고.

	if( func ==  nullptr )
	{
		throw std::logic_error("Not Impl Instruction Set, " + std::to_string( op_code ) );
	}

	func( this, op_code, true );
	if ( isPreFixInstruction )
	{
		return (cycles * 4);
	}

	size_t additional_clock = CheckAdditionalClock( op_code );

	return ( cycles  + additional_clock ) * 4;
}


// PRE 0b00의 콜백 함수.
#define BIND_FUNC( func_name ) static const char * func_name\
( GameboyCPU * cpu, BYTE op_code, bool not_test)\
{\
	if (not_test) {                                            \
        cpu->setConditionResult(false);                                               \
        WORD pc = cpu->GetRegisterPC().reg_16 - 1;                                                    \
		cpu->func_name\
		( op_code );\
		cpu->addInstructionEvent( op_code, { cpu->mRegisters.BC, cpu->mRegisters.DE, cpu->mRegisters.HL, cpu->mRegisters.AF, cpu->mPC, cpu->mSP }, pc, cpu->getConditionResult() );\
		return "";\
	} \
	return #func_name;\
}\


// 바인더.
class BIND_FUNCS
{
public:
	// pre 0b11

	//load
	BIND_FUNC( loadRegSPFromRegHL )
	BIND_FUNC( loadHalfWordMemImm8FromRegA )
	BIND_FUNC( loadHalfWordRegAFromMemImm8 )
	BIND_FUNC( loadMemCFromRegA )
	BIND_FUNC( loadRegAFromMemC )
	BIND_FUNC( loadMemImm16FromRegA )
	BIND_FUNC( loadRegHLFromSPAddSingedImm8 )
	BIND_FUNC( loadRegAFromMemImm16 )

	BIND_FUNC( pushReg16 )
	BIND_FUNC( popReg16 )

	// arth
	BIND_FUNC( addRegAFromImm8 )
	BIND_FUNC( addRegAFromImm8AndCarry )
	BIND_FUNC( addRegSPFromSignedImm8Value )

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

	//jump
	BIND_FUNC( jumpRegister );
	BIND_FUNC( jumpRegisterIfCondition );

	//load
	BIND_FUNC( loadMemoryImm16FromSP )
	BIND_FUNC( loadRegFromImm8 )
	BIND_FUNC( loadMemHLFromImm8 )
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

	//Rotate Register A.
	BIND_FUNC( rotateRightThroughCarryRegisterA )
	BIND_FUNC( rotateLeftThroughCarryRegisterA )

	BIND_FUNC( rotateRightRegisterA )
	BIND_FUNC( rotateLeftRegisterA )

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
	// JR 'singed value'
	mFuncMap[ 0x18 ] = BIND_FUNCS::jumpRegister;
	BYTE opcodes[] = { 0x20, 0x28, 0x30, 0x38 };

	// JR condition 'signed value'
	for ( auto opcode : opcodes )
	{
		mFuncMap[ opcode ] = BIND_FUNCS::jumpRegisterIfCondition;
	}

	// LD reg8, imm8 :  reg8 load imm 8
	// 0b000110 ~ 0b111110
	for(BYTE i = 0b000; i <= 0b111; i++)
	{
		BYTE op_code = 0b00000000u | static_cast<BYTE>( i << 3u ) | 0b110u;

		if ( i == 0b110u )
		{
			mFuncMap[ op_code ] = BIND_FUNCS::loadMemHLFromImm8;
		}
		else
		{
			mFuncMap[ op_code ] = BIND_FUNCS::loadRegFromImm8;
		}
	}

	// LD (WORD) SP
	mFuncMap[ 0x08 ] = BIND_FUNCS::loadMemoryImm16FromSP;

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
	mFuncMap[ 0x07 ] = BIND_FUNCS::rotateLeftThroughCarryRegisterA;
 	mFuncMap[ 0x0F ] = BIND_FUNCS::rotateRightThroughCarryRegisterA;

	mFuncMap[ 0x1F ] = BIND_FUNCS::rotateRightRegisterA;
	mFuncMap[ 0x17 ] = BIND_FUNCS::rotateLeftRegisterA;
}

void GameboyCPU::pre0b01GenerateFuncMap()
{
	// reg8 load reg8
	// 0b000000~0b111111
	for( BYTE i = 0b000; i <= 0b111; i++ )
	{
		for( BYTE j = 0b000; j <= 0b111; j++ )
		{
			if ( ( i == j ) && ( i == 0b110 ) ) { continue; } // (HL) (HL)은 없음..

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

			// reg load reg, 0brrryyy ( rrr, yyy != 0b110)
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

	//LDH (imm8), A
	mFuncMap[ 0xE0 ] = BIND_FUNCS::loadHalfWordMemImm8FromRegA;

	//LDH A, (imm8)
	mFuncMap[ 0xF0 ] = BIND_FUNCS::loadHalfWordRegAFromMemImm8;

	//LD (C), A
	mFuncMap[ 0xE2 ] = BIND_FUNCS::loadMemCFromRegA;

	//LD A, (C)
	mFuncMap[ 0xF2 ] = BIND_FUNCS::loadRegAFromMemC;

	//LD (Imm16), RegA
	mFuncMap[ 0xEA ] = BIND_FUNCS::loadMemImm16FromRegA;

	//LD HL, SP + singed imm 8.
	mFuncMap[ 0xF8 ] = BIND_FUNCS::loadRegHLFromSPAddSingedImm8;

	//LD RegA, (Imm16)
	mFuncMap[ 0xFA ] = BIND_FUNCS::loadRegAFromMemImm16;

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

	//ADD SP, signed 8bit.
	mFuncMap[ 0xE8 ] = BIND_FUNCS::addRegSPFromSignedImm8Value;

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
	mMemoryInterface->Set( mDebugInjectionCount.reg_16, injection_byte );
	mDebugInjectionCount.reg_16 += 1;
}

BYTE GameboyCPU::GetMemoryValue(unsigned int mem_index)
{
	return mMemoryInterface->Get( mem_index );
}

GameboyMemory GameboyCPU::GetMemory()
{
	return GameboyMemory(mMemoryInterface);
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
	mMemoryInterface->Set( mem_index, value );
}

void GameboyCPU::addInstructionEvent(BYTE opcode, std::array<Register, 6> registers, WORD execute_pc_address,
									 bool condition_result)
{

	if ( mOnInstructionCallback != nullptr )
	{
		mOnInstructionCallback( opcode, registers, execute_pc_address, condition_result);
	}
}

// 여기서부터는 명령어들 Helper.

BYTE GameboyCPU::immediateValue()
{
	BYTE value = mMemoryInterface->Get( mPC.reg_16 );
	mPC.reg_16 += 1;
	return value;
}

WORD GameboyCPU::immediateValue16()
{
	const BYTE value_lo = mMemoryInterface->Get( mPC.reg_16 );
	const BYTE value_hi = mMemoryInterface->Get( mPC.reg_16 + 1);

	mPC.reg_16 += 2;

	return  static_cast<WORD>(value_hi << 8u) | value_lo;
}

void GameboyCPU::setWORDToStack(WORD value)
{
	mSP.reg_16 = mSP.reg_16 - 2;
	
	mMemoryInterface->Set( mSP.reg_16 + 1, static_cast<BYTE>((value & 0xff00u) >> 8u) );
	mMemoryInterface->Set( mSP.reg_16, static_cast<BYTE>(value & 0x00ffu));

}

WORD GameboyCPU::getWORDFromStack()
{
	BYTE hi = mMemoryInterface->Get( mSP.reg_16 + 1 );
	BYTE lo = mMemoryInterface->Get( mSP.reg_16 );

	mSP.reg_16 = mSP.reg_16 + 2;

	return (static_cast<WORD>(static_cast<WORD>(hi) << 8u) | static_cast<WORD>(lo));
}

bool GameboyCPU::getIfConditionResult(BYTE op_code) const
{
	BYTE check_condition_param = (op_code & 0b00011000u) >> 3u;
	BYTE check_condition = (check_condition_param & 0b10u) == 0b10u ? GetFlagC() : GetFlagZ();

	if( (check_condition_param & 0b01u) == 1u )
	{
		return check_condition == true;
	}
	else
	{
		return check_condition == false;
	}
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

constexpr BYTE MemoryParamIndex = 0b110;

BYTE GameboyCPU::get8BitArgumentValue(BYTE param)
{
	return (param == MemoryParamIndex) ? mMemoryInterface->Get( mRegisters.HL.reg_16 ) : m8bitArguments[param].ref;
}

void GameboyCPU::set8BitArgumentValue(BYTE param, BYTE value)
{
	if ( param == MemoryParamIndex)
	{
		mMemoryInterface->Set( mRegisters.HL.reg_16, value );
	}
	else
	{
		m8bitArguments[param].ref = value;
	}
}

// 애매해서 여기 둠.
void GameboyCPU::commonAddSPInstructionFlags(char imm8)
{
	setFlagZ( false );
	setFlagN( false );
	setFlagH( (imm8 & 0x0f) + (mSP.reg_16 & 0x0f) > 0xf );
	setFlagC( (imm8 & 0xff) + (mSP.reg_16 & 0xff) > 0xff );
}


void GameboyCPU::setConditionResult(bool value)
{
	mConditionResult = value;
}

bool GameboyCPU::getConditionResult()
{
	return mConditionResult;
}

