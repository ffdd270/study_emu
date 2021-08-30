//
// Created by nhy20 on 2020-06-07.
//

#ifndef GAMEBOY_TYPEDEF_H
#define GAMEBOY_TYPEDEF_H

#include <cstddef>

typedef unsigned char BYTE;
typedef unsigned short WORD;

enum class InterruptsType : WORD
{
	NONE = 0x0u,
	HDMA = 0xff55u,
	DMA = 0xff46u,
	TIMER = 0xff05u,
	LCD_STAT = 0x100,
	V_BLANK = 0x200,
	JOYPAD = 0x300,
};

#endif //GAMEBOY_TYPEDEF_H
