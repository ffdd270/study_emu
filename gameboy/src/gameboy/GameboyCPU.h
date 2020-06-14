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
	bool boot();

	void nextStep();
	void injectionCode();
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
