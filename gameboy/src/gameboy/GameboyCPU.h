//
// Created by nhy20 on 2020-06-07.
//

#ifndef GAMEBOY_GAMEBOYCPU_H
#define GAMEBOY_GAMEBOYCPU_H


#include "typedef.h"

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
	bool Boot();

	void NextStep();



	void InjectionCode();
private:
	// nibble은 반 바이트.
	void nextStep0x0X( BYTE opcode, BYTE second_opcode_nibble );
	void nextStep0x1X( BYTE opcode, BYTE second_opcode_nibble );
	void nextStep0x2X( BYTE opcode, BYTE second_opcode_nibble );
	void nextStep0x3X( BYTE opcode, BYTE second_opcode_nibble );
	void nextStep0x4X( BYTE opcode, BYTE second_opcode_nibble );
	// 0x4~0x7까진 모두 Load R1, R2
	void loadR1R2Instructions( BYTE opcode, BYTE first_opcode_nibble, BYTE second_opcode_nibble );
private:
	// instruction Set.
	void load8Bit( BYTE & to_value, BYTE value );


private:
	BYTE mGameMemory[0x765];


	// 레지스터 영역.
	Register mAF; // Accumulator & Flags. Low 8bit Used by Flag.
	//  mAF Low Bit ->
	//  7 = Zero Flag.
	//  6 = Zero Flag.
	//  5 = Zero Flag.
	//  4 = Carry Flag.
	//  3-0 = Zero Fill. Not Used..

	Register mBC;
	Register mDE;
	Register mHL;
	Register mSP; // Stack Pointer
	Register mPC; // Program Counter

};


#endif //GAMEBOY_GAMEBOYCPU_H
