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
	[[nodiscard]] bool IsLCDDisplayEnable() const; // BIT 7
	[[nodiscard]] WORD GetSelectedWindowTileMap() const; // BIT 6
	[[nodiscard]] bool IsWindowDisplayEnable() const; // BIT 5
	[[nodiscard]] WORD GetSelectBGAndWindowTileData() const; // BIT 4
	[[nodiscard]] WORD GetSelectBGTileMapDisplay() const; // BIT 3
	[[nodiscard]] bool IsSpriteSize() const; // BIT 2
	[[nodiscard]] bool IsSpriteDisplayEnable() const; // BIT 1
	[[nodiscard]] bool CheckProperty() const; // BIT 0

	//LCD Status Register
	[[nodiscard]] bool IsEnableLYCoincidenceInterrupt() const;  // BIT 6
	[[nodiscard]] bool IsEnableMode2OAMInterrupt() const; // BIT 5
	[[nodiscard]] bool IsEnableMode1VBlankInterrupt() const; // BIT 4
	[[nodiscard]] bool IsEnableMode0HBlankInterrupt() const; // BIT 3
	[[nodiscard]] bool GetCoincidenceFlag() const; // BIT 2
	[[nodiscard]] BYTE GetModeFlag() const; // BIT 1-0
private:
	void checkAddress(size_t mem_addr) const;
private:
	BYTE mLCDControlRegister;
	BYTE mLCDStatusRegister;

	std::array<BYTE, 0x2000> mMemory;
};


#endif //GAMEBOY_GPU_H
