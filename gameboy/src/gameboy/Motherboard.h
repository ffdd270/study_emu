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
	enum Interfaces
	{
		Interface_MMUNIT = 0,
		Interface_GPU = 1,
		Interface_ROM = 2,
		Interface_TIMER = 3,
		Interface_JOYPAD = 4,
	};

	Motherboard();

	void Boot();
	void Step();

	void SetCartridge( std::shared_ptr<MemoryInterface> ptr_cartridge );
	std::shared_ptr<MemoryInterface> GetInterface( Interfaces selected_interface );
	std::shared_ptr<GameboyCPU> GetCPU() { return mCPU; }
private:
	void procInterrupts(const std::vector<InterruptsType> & array_interrupt);
	void procInterrupt( InterruptsType interrupt_address );
private:
	std::shared_ptr<GameboyCPU> mCPU;
	std::array<std::shared_ptr<MemoryInterface>, 10> mInterfaces;
};


#endif //GAMEBOY_MOTHERBOARD_H
