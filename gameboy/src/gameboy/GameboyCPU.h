//
// Created by nhy20 on 2020-06-07.
//

#ifndef GAMEBOY_GAMEBOYCPU_H
#define GAMEBOY_GAMEBOYCPU_H


#include "typedef.h"

class GameboyCPU
{
public:
	bool boot();

	void nextStep();
	void injectionCode();
private:
	BYTE mGameMemory[0x765];
};


#endif //GAMEBOY_GAMEBOYCPU_H
