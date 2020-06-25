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



class GameboyCPU
{
public:
	GameboyCPU();

	bool Boot();
	void Reset();

	void NextStep();


	// 게임 보이 디버거 함수들
	void InjectionMemory(BYTE injection_byte);
	void SetMemoryValue( unsigned int mem_index, BYTE value );
	BYTE GetMemoryValue( unsigned int mem_index );

	Register GetRegisterAF() { return mRegisters.array[0]; }
	Register GetRegisterBC() { return mRegisters.array[1]; }
	Register GetRegisterDE() { return mRegisters.array[2]; }
	Register GetRegisterHL() { return mRegisters.array[3]; }



	Register GetRegisterSP() { return mSP; }
	Register GetRegisterPC() { return mPC; }
private:
	/*
	 * 어떻게 명령어를 긁을 것인가?
	*/

	// nibble은 반 바이트.
	void nextStep0x0X( BYTE opcode, BYTE second_opcode_nibble );
	void nextStep0x1X( BYTE opcode, BYTE second_opcode_nibble );
	void nextStep0x2X( BYTE opcode, BYTE second_opcode_nibble );
	void nextStep0x3X( BYTE opcode, BYTE second_opcode_nibble );
	void nextStep0x4X( BYTE opcode, BYTE second_opcode_nibble );

	// 0x4~0x7까진 모두 Load R1, R2
	void loadR1R2Instructions( BYTE opcode, BYTE first_opcode_nibble, BYTE second_opcode_nibble );
	void loadRegtoReg( BYTE opCode );


	/*
	 * CPU 내부 명령들.
	*/

	BYTE immediateValue();
	WORD immediateValue16();

private:
	// instruction Set.

	void load8BitToReg( BYTE & reg_8bit );


private:
	BYTE mGameMemory[0xFFFF];


	union Registers{
		struct
		{
			// 레지스터 영역.
			Register AF; // Accumulator & Flags. Low 8bit Used by Flag.
			//  mAF Low Bit ->
			//  7 = Zero Flag.
			//  6 = Zero Flag.
			//  5 = Zero Flag.
			//  4 = Carry Flag.
			//  3-0 = Zero Fill. Not Used..

			Register BC;
			Register DE;
			Register HL;
		};

		Register array[4];
	};

	Register mPC;
	Register mSP;

	Registers mRegisters;

	struct RefRegister8bit
	{
		BYTE & ref;

		explicit RefRegister8bit(BYTE & ref)  : ref(ref)
		{

		}
	};

	// B, C, D, E, H, L, A 순.
	std::array<RefRegister8bit, 8> m8bitArguments;




	// Debug Register
	Register mDebugInjectionCount;
};


#endif //GAMEBOY_GAMEBOYCPU_H
