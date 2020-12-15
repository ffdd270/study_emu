//
// Created by HaruGakkaP on 2020-12-15.
//

#ifndef GAMEBOY_VRAM_H
#define GAMEBOY_VRAM_H

#include "memory/MemoryInterface.h"
#include <array>

// Video RAM
// 0x8000~0x9fff
class VRAM : public MemoryInterface
{
public:
	VRAM();

	[[nodiscard]] BYTE Get(size_t mem_addr) const override;
	void Set(size_t mem_addr, BYTE value) override;
private:
	void checkAddress(size_t mem_addr) const;
private:
	std::array<BYTE, 0x2000> mMemory;
};


#endif //GAMEBOY_VRAM_H
