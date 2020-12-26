//
// Created by nhy20 on 2020-12-25.
//

#ifndef GAMEBOY_MOTHERBOARD_H
#define GAMEBOY_MOTHERBOARD_H

#include "memory/MemoryInterface.h"
#include <GameboyCPU.h>

#include <memory>
#include <array>

class Motherboard
{
public:
	Motherboard();

	void Boot();
	void Step();

	void SetCartridge( std::shared_ptr<MemoryInterface> ptr_cartridge );
private:
	void procInterrupts(std::array<WORD, 10> & array_interrupt, size_t interrupt_len );
	void procInterrupt( WORD interrupt_address );
private:
	enum Interfaces
	{
		Interface_MMUNIT = 0,
		Interface_GPU = 1,
		Interface_ROM = 2,
	};


	std::shared_ptr<GameboyCPU> mCPU;
	std::array<std::shared_ptr<MemoryInterface>, 10> mInterfaces;
};


#endif //GAMEBOY_MOTHERBOARD_H
