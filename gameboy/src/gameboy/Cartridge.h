//
// Created by HaruGakkaP on 2020-11-23.
//

#ifndef GAMEBOY_CARTRIDGE_H
#define GAMEBOY_CARTRIDGE_H

#include "typedef.h"
#include <vector>
#include <array>
#include <string_view>

struct CartridgeSizeInfo
{
	size_t size = 0;
	size_t bank = 0;
};

class Cartridge
{
public:
	enum class ColorGameBoyFlag
	{
		CGB_NONE = 0x00,
		CGB_SUPPORT = 0x80,
		CGB_ONLY = 0xC0,
		CGB_UNKNOWN = 0xFF
	};

	Cartridge() = default;

	void Load( std::string_view path );
	[[nodiscard]] BYTE GetData( size_t mem_pos ) const;
	[[nodiscard]] BYTE GetRamData( size_t mem_pos ) const;
	void SetRamData( size_t mem_pos, BYTE value );
	[[nodiscard]] std::vector<BYTE> GetRawCartridgeData() const;

	[[nodiscard]] std::string GetTitle() const;
	[[nodiscard]] ColorGameBoyFlag GetCGBFlag() const;
	[[nodiscard]] BYTE GetCartridgeType() const;
	[[nodiscard]] CartridgeSizeInfo GetRamSizeInfo() const;
	[[nodiscard]] CartridgeSizeInfo GetSizeInfo() const;
	[[nodiscard]] size_t GetRealBufferSize() const { return mBuffer.size(); }
	[[nodiscard]] std::array<BYTE, 4> GetEntryPoint() const;
private:
	void ramErrorCheck( size_t pos ) const;
	void basicErrorCheck( size_t pos ) const;
private:
	std::vector<BYTE> mBuffer;
	std::vector<BYTE> mRam;
};


#endif //GAMEBOY_CARTRIDGE_H
