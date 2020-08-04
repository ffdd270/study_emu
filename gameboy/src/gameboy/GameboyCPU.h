//
// Created by nhy20 on 2020-06-07.
//

#ifndef GAMEBOY_GAMEBOYCPU_H
#define GAMEBOY_GAMEBOYCPU_H


#include "typedef.h"
#include <array>

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

class GameboyCPU
{
public:
	friend class BIND_FUNCS;

	GameboyCPU();

	bool Boot();
	void Reset();

	void NextStep();


	// 게임 보이 디버거 함수들
	void InjectionMemory(BYTE injection_byte);
	void SetMemoryValue( unsigned int mem_index, BYTE value );
	BYTE GetMemoryValue( unsigned int mem_index );

	Register GetRegisterAF() { return mRegisters.array[3]; }
	Register GetRegisterBC() { return mRegisters.array[0]; }
	Register GetRegisterDE() { return mRegisters.array[1]; }
	Register GetRegisterHL() { return mRegisters.array[2]; }

	Register GetRegisterSP() { return mSP; }
	Register GetRegisterPC() { return mPC; }

	BYTE GetFlagZ() { return ( mRegisters.AF.lo & 0b10000000 ) >> 7; }
	BYTE GetFlagN() { return ( mRegisters.AF.lo & 0b01000000 ) >> 6; }
	BYTE GetFlagH() { return ( mRegisters.AF.lo & 0b00100000 ) >> 5; }
	BYTE GetFlagC() { return ( mRegisters.AF.lo & 0b00010000 ) >> 4; }

private:
	std::array<BindFunctionPointer, 0xFF> mFuncMap;
private:
	void pre0b00GenerateFuncMap();
	void pre0b01GenerateFuncMap();
	void pre0b10GenerateFuncMap();
	void pre0b11GenerateFuncMap();

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
	void loadRegFromReg(BYTE opCode );

	// LD r, n (2)
	// 0b00rrr110
	// 0bnnnnnnnn
	void loadRegFromImm8(BYTE opCode );

	// LD r, (IX+d)
	// 게임보이는 그런 거 없음

	// LD r, (IY+d)
	// 게임보이는 그런 거 없음

	// LD r, ( HL ) (1)
	// 0b01rrr110
	void loadRegFromMemHL(BYTE opCode );


	//LD (IX+d), r
	//게임보이는 그런 거 없음

	//LD (IY+d), r
	//역시 게임보이는 그런 거 없음

	// LD (HL), r (1)
	// 0b01110rrr
	// (HL)<-r
	void loadMemHLFromReg(BYTE opCode );

	//LD (IX+d), n
	//게임보이는 그런 거 없음

	//LD (IY+d), n
	//게임보이는 그런 거 없음

	//LD A, (BC) (1)
	// 0b00001010 (0x0A)
	// A<-(BC)
	void loadRegAFromMemBC(BYTE opCode );


	//LD A, (DE)
	// 0b00011010 (0x1A)
	// A<-(DE)
	void loadRegAFromMemDE(BYTE opCode);

	//LD A, (HL+)
	// 0b00101010 (0x2A)
	// A<-(HL), HL<-HL + 1
 	void loadRegAFromMemHLAndIncHL(BYTE opCode);


	//LD A, (nn)
	// 0b00111010 (0x3A) (only on CHIP-8)
	// 0bnnnnnnnn
	// 0bnnnnnnnn
	// A<-(nn)
	void loadRegAFromMemNN(BYTE opCode);

	//LDD A,(HL)
	// 0b00111010 (0x3A) (only on Gameboy CPU. )
	// A<-(HL) and HL--;
	void loadRegAFromMemHLAndDecHL(BYTE opCode);

	//LD (BC), A (1)
	// 0b00000010 (0x02)
	// (BC)<-A
	void loadMemBCFromRegA(BYTE opCode );

	//LD (DE), A (1)
	// 0b00010010 (0x12)
	// (DE)<-A
	void loadMemDEFromRegA(BYTE opCode);

	//LD (HL+), A ( or LDI HL, A ) (1)
	// 0b00100010 (0x22)
	// (HL+)<-A and HL<-HL + 1
	void loadMemHLFromRegAAndIncHL(BYTE opCode);

	//LD (HL-) , A( or LDD HL, A ) (1) - (Only On Gameboy CPU)
	// 0b00110010 (0x32)
	// HL<-A and HL<-HL - 1
	void loadMemHLFromRegAAndDecHL(BYTE opCode);

	//LD (nn), A(3) - (Only On Z80)
	// 0b00110010 0x32
	// 0bnnnnnnnn
	// 0bnnnnnnnn
	void loadMemNNFromRegA(BYTE opCode);

	/// 여기서부터 16비트 로드 명령어

	//LD DD, RR (3)
	//0b00dd0001
	//Imm
	//Imm
	// DD <- Imm16
	void loadReg16FromImm16( BYTE opCode );


	//LD HL, (nn) (3) - (Only On Z80)
	//0b00101010 0x2A
	//0bnnnnnnnn
	//0bnnnnnnnn
	// H <- (nn + 1), L <- (nn)
	void loadRegHLFromMemNN16( BYTE opCode );


	//LD SP, HL
	// 0b11111001 0xF9
	// SP <- HL
	void loadRegSPFromRegHL( BYTE opCode );

	//PUSH qq
	// 0b11qq0101 ( qq = { BC = 00, DE = 01, HL = 10, AF = 11 }
	// (SP - 2) <- qqLow, (SP - 1) <- qqHi, SP<-SP - 2
	void pushReg16( BYTE opCode );

	//POP qq
	// 0b11qq0001 ( qq = { BC = 00, DE = 01, HL = 10, AF = 11 } }
	// qqH <- (SP + 1), qqL <- (SP)
	void popReg16( BYTE opCode );


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
	void addRegAFromRegister( BYTE opCode );

	//ADD A, n
	// A <- A + n
	// 0b11000110 ( 0xC6 )
	// imm8
	// = Flag = ( Same as ADD A, r )
	void addRegAFromImm8(BYTE opCode);

	//ADD A, (HL)
	// 0b10000110 ( 0x86 )
	// = Flag = ( Same as ADD A, r )
	void addRegAFromMemHL(BYTE opCode);

	//ADC A, r ( Add With Carry. if Carry Set. add + 1 from result value. )
	// 0b10001rrr (r = m8BitArguments)
	// = Flag = ( Same as ADD A, r )
	void addRegAFromRegisterAndCarry(BYTE opCode);

	//ADC A, n ( Add With Carry. if Carry Set. add + 1 from result value. )
	// 0b11001110 ( 0xCE )
	// 0bnnnnnnnn
	// = Flag = ( Same as ADD A, r )
	void addRegAFromImm8AndCarry(BYTE opCode);

	//ADC A, (HL) (  Add With Carry. if Carry Set. add + 1 from result value. )
	// 0b10001110 (0x8E)
	// = Flag = ( Same as ADD A, r )
	void addRegAFromMemHLAndCarry(BYTE opCode);


	//SUB r
	// 0b10010rrr { r = m8BitArguments }
	// = Flag =
	// Z -> 결과값 0.
	// H -> 하위 4비트 연산 결과가 음수.
	// C -> 연산 결과가 음수.
	void subRegAFromRegister(BYTE opCode);

	//SUB n
	// 0b11010110 (0xD6)
	// 0bnnnnnnnn
	// = Flag = ( Same as SUB r )
	void subRegAFromImm8(BYTE opCode);


	//SUB (HL)
	// 0b10010110 (0x96)
	// = Flag = ( Same as SUB r )
	void subRegAFromMemHL(BYTE opCode);


	//SBC A, r
	// 0b10011rrr { r = m8BitArguments }
	// = Flag = ( Same as SUB r )
	void subRegAFromRegisterAndCarry(BYTE opCode);

	//SBC A, n
	// 0b11011110 (0xDE)
	// = Flag = ( Same as SUB r )
	void subRegAFromImm8AndCarry(BYTE opCode);

	//SBC A, (HL)
	// 0b10011110 (0x9E)
	// = Flag = ( Same as SBC A, (HL)
	void subRegAFromMemHLAndCarry(BYTE opCode);

	//AND r
	// 0b10100rrr
	// = Flag =
	// Z = if 0 set 0.
	// else = reset.
	void andRegAFromRegister(BYTE opCode);

	//AND n
	// 0b11100110 (0xE6)
	// = Flag = Same as AND r
	void andRegAFromImm8( BYTE opCode );

	//AND (HL)
	// 0b10100110 (0xA6)
	// = Flag = Same as AND (HL)
	void andRegAFromMemHL( BYTE opCode );

	/*
	 * Common 함수들. 로직은 똑같은데 Flag에 따른 변화가 있을 경우 , 공용 부분은 이쪽에서..
	 */

	void commonAddRegAFromRegister( BYTE opCode, BYTE carry);
	void commonAddRegAFromImm8(BYTE opCode, BYTE carry );
	void commonAddRegAFromMemHL(BYTE opCode, BYTE carry);

	void commonSubRegAFromRegister(BYTE opCode, BYTE carry);
	void commonSubRegAFromImm8(BYTE opCode, BYTE carry);
	void commonSubRegAFromMemHL(BYTE opCode, BYTE carry);

	void commonAddSetFlag( BYTE origin_value, BYTE add_value, BYTE carry );
	void commonSubSetFlag( BYTE origin_value, BYTE sub_value, BYTE carry );
	void commonAndSetFlag();
	/*
	 * Util 함수들.
	*/


	BYTE immediateValue();
	WORD immediateValue16();

	void setFlagZ( bool flag );
	void setFlagN( bool flag );
	void setFlagH( bool flag );
	void setFlagC( bool flag );

	void resetFlags();

private:
	BYTE mGameMemory[0xFFFF];


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

	// Debug Register
	Register mDebugInjectionCount;
};


#endif //GAMEBOY_GAMEBOYCPU_H
