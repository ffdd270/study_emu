//
// Created by HaruGakkaP on 2020-12-15.
//

#ifndef GAMEBOY_GPU_H
#define GAMEBOY_GPU_H

#include "memory/MemoryInterface.h"
#include <array>

// Video RAM
// 0x8000~0x9fff
class GPU final : public MemoryInterface
{
public:
	GPU();

	[[nodiscard]] BYTE Get(size_t mem_addr) const override;
	void Set(size_t mem_addr, BYTE value) override;

	~GPU() final = default;

	//LCD Control Register
	bool IsLCDDisplayEnable(); // BIT 7
	WORD GetSelectedWindowTileMap(); // BIT 6
	bool IsWindowDisplayEnable(); // BIT 5
	WORD GetSelectBGAndWindowTileData(); // BIT 4
	WORD GetSelectBGTileMapDisplay(); // BIT 3
	bool IsSpriteSize(); // BIT 2
	bool IsSpriteDisplayEnable(); // BIT 1
	bool CheckProperty(); // BIT 0

	//LCD Status Register
	bool IsEnableLYCoincidenceInterrupt();  // BIT 6
	bool IsEnableMode2OAMInterrupt(); // BIT 5
	bool IsEnableMode1VBlankInterrupt(); // BIT 4
	bool IsEnableMode0HBlankInterrupt(); // BIT 3
	bool GettCoincidenceFlag(); // BIT 2
	BYTE GetModeFlag(); // BIT 1-0
private:
	void checkAddress(size_t mem_addr) const;
private:
	BYTE mLCDControlRegister;
	BYTE mLCDStatusRegister;

	std::array<BYTE, 0x2000> mMemory;
};


#endif //GAMEBOY_GPU_H
