//
// Created by HaruGakkaP on 2020-11-30.
//

#ifndef GAMEBOY_MBC1_H
#define GAMEBOY_MBC1_H

#include "Cartridge.h"

/*
 * Gameboy의 기본적인 Memory Controller.
 *
 *
 */


class MBC1
{
private:
	MBC1( Cartridge && cartridge );

	BYTE Get(size_t mem_addr);
	void Set(size_t mem_addr);
public:
	Cartridge mCartridge;
};


#endif //GAMEBOY_MBC1_H
