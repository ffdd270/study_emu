//
// Created by nhy20 on 2020-06-07.
//

#ifndef GAMEBOY_GAMEBOYCPU_H
#define GAMEBOY_GAMEBOYCPU_H


#include "typedef.h"
#include <array>
#include <vector>
#include <functional>

union Register
{
	WORD reg_16;

	struct
	{
		BYTE lo, hi;
	};
};


class GameboyCPU;

typedef void(*BindFunctionPointer)(GameboyCPU *, BYTE);
using InstructionCallback = std::function<void( const char * instruction_name, BYTE opcode )>;


// 외부 반출용
class GameboyMemory
{
public:
	friend class GameboyCPU;

	BYTE GetValue( std::size_t memory_index )
	{
		if ( memory_index >= mSize )
		{
			throw std::logic_error("OUT OF INDEX");
		}

		return mPtrMemory[memory_index];
	}
private:
	GameboyMemory( BYTE * ptr_memory, std::size_t mem_size ) : mPtrMemory( ptr_memory ), mSize( mem_size )  { }

	BYTE * mPtrMemory;
	std::size_t mSize;
};



class GameboyCPU
{
public:
	friend class BIND_FUNCS;

	GameboyCPU();

	void Reset();

	void NextStep();
	[[nodiscard]] bool IsInterruptEnable() const { return  mInturruptEnable; }

	// 게임 보이 디버거 함수들
	void AddBreakPoint( WORD break_address );
	void RemoveBreakPoint( WORD break_address );
	void ContinueFromBreakPoint();

	void InjectionMemory(BYTE injection_byte);
	void SetInjectionCount(WORD injection_address) { mDebugInjectionCount.reg_16 = injection_address; }

	void SetOnInstructionCallback( InstructionCallback callback );
	void RemoveInstructionCallback();
	void SetMemoryValue( unsigned int mem_index, BYTE value );
	BYTE GetMemoryValue( unsigned int mem_index );
	GameboyMemory GetMemory();

	BYTE GetRegisterValueBy8BitIndex( BYTE index ) const {  return m8bitArguments[index].ref; }
	Register GetRegisterAF() { return mRegisters.array[3]; }
	Register GetRegisterBC() { return mRegisters.array[0]; }
	Register GetRegisterDE() { return mRegisters.array[1]; }
	Register GetRegisterHL() { return mRegisters.array[2]; }

	Register GetRegisterSP() { return mSP; }
	Register GetRegisterPC() { return mPC; }

	[[nodiscard]] BYTE GetFlagZ() const { return ( mRegisters.AF.lo & 0b10000000u ) >> 7u; }
	[[nodiscard]] BYTE GetFlagN() const { return ( mRegisters.AF.lo & 0b01000000u ) >> 6u; }
	[[nodiscard]] BYTE GetFlagH() const { return ( mRegisters.AF.lo & 0b00100000u ) >> 5u; }
	[[nodiscard]] BYTE GetFlagC() const { return ( mRegisters.AF.lo & 0b00010000u ) >> 4u; }

	[[nodiscard]] bool IsHalted() const { return mHalted; }

private:
	std::array<BindFunctionPointer, 0xFF + 1> mFuncMap;
	std::array<BindFunctionPointer, 0xFF + 1> mPrefixCBFuncMap;
private:
	void pre0b00GenerateFuncMap();
	void pre0b01GenerateFuncMap();
	void pre0b10GenerateFuncMap();
	void pre0b11GenerateFuncMap();

	void pre0xCBGenerateFuncMap();
private:
	// 명령어 구현 부

	// 작명 규칙
	// void <명령어><dest 인자>From<origin 인자>
	// Reg = Register 8 bit.
	// Mem  = memory value
	// (NN) = NN 레지스터 값 위치에 있는 게임 메모리 값.
	// Reg16 = Register 16bit.
	// Imm = 임시값,  코드에 박혀 있는 값.
	// Imm16 = 16비트 임시값. hi, lo로 잘려서 있음.

	// Z80 Cpu 메뉴얼을 기초로 만들었지만, 0xED등 2바이트 명령어들은 게임보이 CPU에서 지원하지 않아 구현에서 제외.


	// 주석은
	// <명렁어> <인자> <인자>  <(명령어 길이)>

	// 여기서부터 로드 명령어 집합

	// LD r, r' (1)
	// 0b01rrryyy
	void loadRegFromReg(BYTE op_code );

	// LD r, n (2)
	// 0b00rrr110
	// 0bnnnnnnnn
	void loadRegFromImm8(BYTE op_code );

	// LD r, ( HL ) (1)
	// 0b01rrr110
	void loadRegFromMemHL(BYTE op_code );


	// LD (HL), r (1)
	// 0b01110rrr
	// (HL)<-r
	void loadMemHLFromReg(BYTE op_code );

	//LD A, (BC) (1)
	// 0b00001010 (0x0A)
	// A<-(BC)
	void loadRegAFromMemBC(BYTE op_code );


	//LD A, (DE)
	// 0b00011010 (0x1A)
	// A<-(DE)
	void loadRegAFromMemDE(BYTE op_code);

	//LD A, (HL+)
	// 0b00101010 (0x2A)
	// A<-(HL), HL<-HL + 1
 	void loadRegAFromMemHLAndIncHL(BYTE op_code);

	//LDD A,(HL)
	// 0b00111010 (0x3A) (only on Gameboy CPU. )
	// A<-(HL) and HL--;
	void loadRegAFromMemHLAndDecHL(BYTE op_code);

	//LD (BC), A (1)
	// 0b00000010 (0x02)
	// (BC)<-A
	void loadMemBCFromRegA(BYTE op_code );

	//LD (DE), A (1)
	// 0b00010010 (0x12)
	// (DE)<-A
	void loadMemDEFromRegA(BYTE op_code);

	//LD (HL+), A ( or LDI HL, A ) (1)
	// 0b00100010 (0x22)
	// (HL+)<-A and HL<-HL + 1
	void loadMemHLFromRegAAndIncHL(BYTE op_code);

	//LD (HL-) , A( or LDD HL, A ) (1) - (Only On Gameboy CPU)
	// 0b00110010 (0x32)
	// HL<-A and HL<-HL - 1
	void loadMemHLFromRegAAndDecHL(BYTE op_code);

	/// 여기서부터 16비트 로드 명령어

	//LD DD, RR (3)
	//0b00dd0001
	//Imm
	//Imm
	// DD <- Imm16
	void loadReg16FromImm16( BYTE op_code );

	//LD SP, HL
	// 0b11111001 0xF9
	// SP <- HL
	void loadRegSPFromRegHL( BYTE op_code );

	//PUSH qq
	// 0b11qq0101 ( qq = { BC = 00, DE = 01, HL = 10, AF = 11 }
	// (SP - 2) <- qqLow, (SP - 1) <- qqHi, SP<-SP - 2
	void pushReg16( BYTE op_code );

	//POP qq
	// 0b11qq0001 ( qq = { BC = 00, DE = 01, HL = 10, AF = 11 } }
	// qqH <- (SP + 1), qqL <- (SP)
	void popReg16( BYTE op_code );

	/*
	 * 8비트 산술 명령어
	 */

	//ADD A, r
	// 0b10000rrr { rrr = 8bitArgument }
	// A <- A + r
	// = Flag =
	// Z = if A is 0
	// H = if bit 3 carry
	// C = if bit 7 carry
	// N = Reset
	void addRegAFromRegister( BYTE op_code );

	//ADD A, n
	// A <- A + n
	// 0b11000110 ( 0xC6 )
	// imm8
	// = Flag = ( Same as ADD A, r )
	void addRegAFromImm8(BYTE op_code);

	//ADD A, (HL)
	// 0b10000110 ( 0x86 )
	// = Flag = ( Same as ADD A, r )
	void addRegAFromMemHL(BYTE op_code);

	//ADC A, r ( Add With Carry. if Carry Set. add + 1 from result value. )
	// 0b10001rrr (r = m8BitArguments)
	// = Flag = ( Same as ADD A, r )
	void addRegAFromRegisterAndCarry(BYTE op_code);

	//ADC A, n ( Add With Carry. if Carry Set. add + 1 from result value. )
	// 0b11001110 ( 0xCE )
	// 0bnnnnnnnn
	// = Flag = ( Same as ADD A, r )
	void addRegAFromImm8AndCarry(BYTE op_code);

	//ADC A, (HL) (  Add With Carry. if Carry Set. add + 1 from result value. )
	// 0b10001110 (0x8E)
	// = Flag = ( Same as ADD A, r )
	void addRegAFromMemHLAndCarry(BYTE op_code);


	//SUB r
	// 0b10010rrr { r = m8BitArguments }
	// = Flag =
	// Z -> 결과값 0.
	// H -> 하위 4비트 연산 결과가 음수.
	// C -> 연산 결과가 음수.
	void subRegAFromRegister(BYTE op_code);

	//SUB n
	// 0b11010110 (0xD6)
	// 0bnnnnnnnn
	// = Flag = ( Same as SUB r )
	void subRegAFromImm8(BYTE op_code);


	//SUB (HL)
	// 0b10010110 (0x96)
	// = Flag = ( Same as SUB r )
	void subRegAFromMemHL(BYTE op_code);


	//SBC A, r
	// 0b10011rrr { r = m8BitArguments }
	// = Flag = ( Same as SUB r )
	void subRegAFromRegisterAndCarry(BYTE op_code);

	//SBC A, n
	// 0b11011110 (0xDE)
	// = Flag = ( Same as SUB r )
	void subRegAFromImm8AndCarry(BYTE op_code);

	//SBC A, (HL)
	// 0b10011110 (0x9E)
	// = Flag = ( Same as SUB r )
	void subRegAFromMemHLAndCarry(BYTE op_code);

	//AND r
	// 0b10100rrr
	// = Flag =
	// Z = if 0 set 0.
	// else = reset.
	void andRegAFromRegister(BYTE op_code);

	//AND n
	// 0b11100110 (0xE6)
	// = Flag = Same as AND r
	void andRegAFromImm8( BYTE op_code );

	//AND (HL)
	// 0b10100110 (0xA6)
	// = Flag = Same as AND r
	void andRegAFromMemHL( BYTE op_code );


	//OR r
	// 0b10110rrr { r = m8BitArguments }
	// = Flag = Same as AND r
	void orRegAFromRegister(BYTE op_code);

	//OR n
	// 0b11110110 ( 0xF6 )
	// = Flag = Same as AND r
	void orRegAFromImm8(BYTE op_code);

	//OR (HL)
	// 0b10110110 ( 0xB6 )
	// = Flag = Same as AND r
	void orRegAFromMemHL(BYTE op_code);

	//XOR r
	// 0b10101rrr { r = m8BitArguments }
	// = Flag =
	// Z = if 0 set 0.
	// else = reset.
	void xorRegAFromRegister( BYTE op_code );

	//XOR n
	// 0b11101110 ( 0xEE )
	// = Flag = Same as XOR r
	void xorRegAFromImm8( BYTE op_code );

	//XOR (HL)
	// 0b10101110 ( 0xAE )
	// = Flag = Same as XOR r
	void xorRegAFromMemHL( BYTE op_code );

	//CP r
	// Compare N. Sub A - n, but result is only flag. a is not changed.
	// 0b10111rrr { r = m8BitArguemnts }
	// = Flag = Same as SUB r
	void cpRegAFromRegister( BYTE op_code );

	//CP n
	// 0b11111110 (0xFE)
	// = Flag = Same as SUB r
	void cpRegAFromImm8( BYTE op_code );

	//CP (HL)
	// 0b10111110 (0xBE)
	// = Flag = Same as SUB r
	void cpRegAFromMemHL( BYTE op_code );

	//INC r
	// 0b00rrr100 { r = m8BitArguments }
	// = Flag = Same as ADD instruction
	void incRegister(BYTE op_code);

	//INC (HL)
	// (HL)++
	// 0b00110100 ( 0x34 )
	// = Flag = Same as ADD instruction
	void incMemHL(BYTE op_code);

	//DEC r
	// r--
	// 0b00rrr101 { r = m8BitArguments }
	// = Flag = Same as Sub Instruction
	void decRegister(BYTE op_code);

	//DEC (HL)
	// (HL)--
	// 0b00110101 (0x35)
	// = Flag = Same as Sub Instruction
	void decMemHL(BYTE op_code);

	/*
	 * 16비트 산술 명령어.
	 */

	//ADD (HL) reg16
	// 0b00rr1001 { r = m16BitArguments }
	// C => Carry?
	// H => Bit 11 Carry?  ( Result More than Half ? )
	void addHLFromReg16(BYTE op_code);

	// INC ss
	// 0b00rr0011 { r = m16BitArguments ]
	// No Bit Effect.
	void incReg16( BYTE op_code );

	// DEC ss
	// 0b00rr1011 { r = m16BitArguments }
	// No Bit Effect
	void decReg16( BYTE op_code );

	// General-Purpose Arithmetic and CPU Control Groups.

	//DAA ( Decimal Adjust Accumulator )
	// A -> Translate To BCD, After Arithmetic Instruction.
	// 0b00100111 ( 0x27 )
	// Flag
	// H = 0
	// C = 이전 연산 결과 유지.
	// DAA의 특징은..
	// 0x15 + 0x27 != 3C, = 42.
	// 마치 정수 연산처럼 작용한다.
	void decimalAdjustRegisterA(BYTE op_code);

	//CPL (complement register)
	// A -> ~A
	// 0x2F
	// H = Set
	// N = Set
	void complementRegister( BYTE op_code );

	//CCF ( Compliment Carry Flag  )
	// CF -> ~CF  ( Carry Flag )
	// 0x3f
	// H = Copy of previous C
	// C = Not C
	void complementCarryFlag( BYTE op_code );

	//SCF ( Set Carry Flag )
	// CF -> true
	// 0x37
	// H = Reset
	// N = Reset
	// C = Set
	void setCarryFlag( BYTE op_code );

	//HALT
	// Halt -> true.
	// 0x76
	void halt( BYTE op_code );


	//DI
	// Desc
	// CPU가 명령어 'DI'를 실행할 때 마스크 가능한 인터럽트는 'EI'명령어에 의해 다시 활성화 될 때까지 비활성화됩니다.
	// CPU가 인터럽트 요청 (INT) 신호에 응답하지 않습니다.
	// 0xf3 ( pre 0b11 )
	void disableInterrupt( BYTE op_code );

	//EI
	// Desc
	// CPU가 EI RETI 명령어를 실행하면, RETI 명렁을 실행할 때 마스크 가능한 인터럽트가 활성화 됩니다.
	// 0xfb ( pre 0b11 )
	void enableInterrupt( BYTE op_code );

	// Prefix CB
	// 0xcb
	// 0xnn (여기가 command.)

	// BIT Shift, Rotate, SWAP Group.
	//RLC
	// Desc
	// Rotate Left. Carry is Bit7, Bit0 is Carry.
	// 0xcb, 0x00000rrr { r = m8BitRegisters } ( HL = 0b110 )
	void rotateLeftThroughCarry( BYTE op_code );

	//RRC
	// Decs
	// Rotate Right. Carry is Bit0, Bit7 is Carry.
	// 0xcb, 0b00001rrr { r = m8BitRegisters } ( HL = 0b110 )
	void rotateRightThroughCarry( BYTE op_code );

	//RRCA
	// Desc
	// RRC, but Register A, and Not Prefix.
	// Reset Z, N, H. if Carry Set Carry.
	// Op Code
	// 0x0f
	void rotateRightThroughCarryRegisterA( BYTE op_code );

	//RL
	// Desc
	// Bit0 is Carry. Carry is Bit7. and Rotate Left.
	// 0xcb, 0b00010rrr { r = m8BitRegisters }
	void rotateLeft( BYTE op_code );

	//RR
	// Desc
	// Bit7 is Carry. Carry is Bit0. and Rotate Right.
	// 0xcb, 0b00011rrr
	void rotateRight( BYTE op_code );

	//RRA
	// Desc
	// RRC, but Register A, and Not Prefix.
	// Reset Z, N, H. if Carry Set Carry.
	// Op Code
	// 0x1f
	void rotateRightRegisterA( BYTE op_code );

	//SLA
	// Desc
	// Carry is Bit7. Shift Left.
	// 0xcb, 0b00100rrr { r = m8BitRegisters }
	void shiftLeftArithmetic(BYTE op_code);  //왜 Accumulator?

	//SRA
	// Desc
	// Carry is Bit0, Shift Right, Bit7 retain Origin Value.
	// 0xcb, 0b00101rrr { r = m8BitRegisters }
	void shiftRightArithmetic(BYTE op_code);

	// SWAP
	// Desc
	// All Flag Reset, Z Flag set if result is zero. Change Low 4 Bit <-> High 4 Bit.
	// 0xcb, 0b00110rrr { r = m8BitRegisters }
	void swapLowAndHigh(BYTE op_code);

	//SRL
	// Desc
	// Carry is Bit0. Shift Right.
	// 0xcb, 0b00111rrr { r = m8BitRegisters }
	void shiftRightLogical(BYTE op_code);

	// BIT Set, Reset, Test Instruction set. ( Still 0xCB )

	//BIT
	// Desc
	// Z is Bit B in Register R ( or (HL) )
	// 0xcb, 0b01bbbrrr { b = B. bit position }, { r = m8BitRegisters }
	void bitTest(BYTE op_code);

	//SET
	// Desc
	// BIT B in Register R set as 1.
	// 0xcb, 0b11bbbrrr { b = B. bit position }, { r = m8BitRegisters }
	void setBit(BYTE op_code);

	//RES
	// Desc
	// BIT B in Register R reset( 0 )
	// 0xcb, 0b10bbbrrr { b = B. bit position }, { r = m8BitRegisters }
	void resetBit(BYTE op_code);

	//Jump Group.

	// Instruction sets.
	// JP C, word => 0xda
	// JP NC, word => 0xd2
	// JP Z, word => 0xca
	// JP NZ, word  => 0xc2
	// Desc
	// If Condition Check Pass, JP to word.
	void jumpToWordIfCondition(BYTE op_code);

	// JP a16
	// Desc
	// PC is a16.
	// 0xC3
	void jumpToWord(BYTE op_code);

	//JP (HL)
	// Desc
	// PC is HL. Not (HL) (Memory.)
	// 0xE9
	void jumpToHL(BYTE op_code);

	//CALL nn
	// Desc
	// (SP - 1) = PC High
	// (SP - 2) = PC Low.
	// PC = WORD
	// SP = SP - 2
	// Op Code
	// 0xCD, 0xWordHI, 0xWordLow
	void callWord(BYTE op_code);

	//CALL cc, nn
	// Param
	// cc = { 00 = NZ, 01 = Z, 10 = NC, 11 = C }
	// Desc
	// if cc was true, CALL nn.
	// Op Code
	// 0b110cc100, 0xWordHI, 0xWordLow
	void callIfCondition(BYTE op_code);

	//RET
	// Desc
	// PC = WORD { hi = (SP - 2) lo = (SP - 1) }
	// SP = SP + 2
	// Op Code
	// 0xC9
	void returnInstruction(BYTE op_code);


	//RET cc
	// Param
	// cc = { 00 = NZ, 01 = Z, 10 = NC, 11 = C }
	// Desc
	// if cc was true, RET.
	// Op Code
	// 0b110cc000
	void returnIfCondition(BYTE op_code);

	//RETI
	// Desc
	// Return From Interrupt.
	// EI = true
	// PC = RET Instruction
	// Op Code
	// 0xD9
	void returnFromInterrupt(BYTE op_code);

	//RST p
	// Desc
	// RESTART from P, Current PC push to Stack.
	// Param
	// ppp = { 000 = 00h, 001 = 08h, 010 = 10h, 011 = 18h, 100 = 20h, 101 = 28h, 110 = 30h, 111 = 38h }
	// Op Code
	// 0b11ppp111
	void restartFromParam(BYTE op_code);

	// Friend로 호출되는 Callback 이벤트
	void addInstructionEvent(const char * name, BYTE opcode);

	/*
	 * Common 함수들. 로직은 똑같은데 Flag에 따른 변화가 있을 경우 , 공용 부분은 이쪽에서..
	 */

	void commonAddRegAFromRegister( BYTE op_code, BYTE carry);
	void commonAddRegAFromImm8(BYTE op_code, BYTE carry );
	void commonAddRegAFromMemHL(BYTE op_code, BYTE carry);

	void commonSubRegAFromRegister(BYTE op_code, BYTE carry);
	void commonSubRegAFromImm8(BYTE op_code, BYTE carry);
	void commonSubRegAFromMemHL(BYTE op_code, BYTE carry);

	void commonAddSetFlag( BYTE origin_value, BYTE add_value, BYTE carry );
	void commonSubSetFlag( BYTE origin_value, BYTE sub_value, BYTE carry );

	void commonRotateRightThroughCarry( BYTE & ref_value );
	void commonRotateRight( BYTE & ref_value );

	void commonBitSetFlag();

	void commonRotateAndShiftFlags(const BYTE & result_value );
	/*
	 * Util 함수들.
	*/


	BYTE immediateValue();
	WORD immediateValue16();

	void setWORDToStack( WORD value );
	WORD getWORDFromStack();

	bool getIfConditionResult( BYTE op_code ) const;

	void setFlagZ( bool flag );
	void setFlagN( bool flag );
	void setFlagH( bool flag );
	void setFlagC( bool flag );

	void resetFlags();

private:
	BYTE mGameMemory[0xFFFF];

	bool mHalted;

	BYTE& get8BitArgumentValue( BYTE param );

	union Registers{
		// 레지스터 영역.
		struct
		{
			Register BC;
			Register DE;
			Register HL;

			Register AF; // AccumulaFromr & Flags. Low 8bit Used by Flag.
			//  mAF Low Bit ->
			//  7 = // Z  : Zero Falg.
			//  6 = // N = SUbtract Flag
			//  5 = Zero Flag. // H
			//  4 = Carry Flag. // C
			//  3-0 = Zero Fill. Not Used..
		};

		Register array[4];
	};

	Register mPC;
	Register mSP;

	Registers mRegisters;


// 이 구조체는 반드시 스코프 안에서 파기 되어야 합니다 . 객체간의 이동을 고려하고 만들지 않았습니다.
	struct RefRegister8bit
	{
		BYTE & ref;

		explicit RefRegister8bit(BYTE & ref)  : ref(ref)
		{

		}
	};

	// 외부 전달 금지
	struct RefRegister16Bit
	{
		WORD & ref;

		explicit RefRegister16Bit( WORD & ref ) : ref( ref )
		{

		}
	};

	// B, C, D, E, H, L, (ERROR), A 순.
	std::array<RefRegister8bit, 8> m8bitArguments;

	// BC, DE, HL, SP
	std::array<RefRegister16Bit, 4> m16bitArguments;

	InstructionCallback mOnInstructionCallback;

	// Debug Register
	Register mDebugInjectionCount;

	// Debug Break Points
	std::unordered_map<WORD, bool> mBreakPoints;

	bool mInturruptEnable;
};


#endif //GAMEBOY_GAMEBOYCPU_H
