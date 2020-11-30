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
public:
	MBC1( Cartridge && cartridge );

	BYTE Get(size_t mem_addr);
	void Set(size_t mem_addr);

private:
	void setActiveRAM( size_t mem_addr );
	void setRomBankLeast5Bit( size_t mem_addr );
	void setRomBankHigh3Bit( size_t mem_addr );
	void setBankMode( size_t mem_addr );
private:
	Cartridge mCartridge;
};


#endif //GAMEBOY_MBC1_H
