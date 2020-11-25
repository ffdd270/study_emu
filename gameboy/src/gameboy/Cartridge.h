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
	BYTE GetData( size_t mem_pos );
	std::vector<BYTE> GetRawCartridgeData();

	std::string GetTitle();
	ColorGameBoyFlag GetCGBFlag();
	BYTE GetCartridgeType();
	CartridgeSizeInfo GetSizeInfo();
	size_t GetRealBufferSize() { return mBuffer.size(); }
	std::array<BYTE, 4> GetEntryPoint();
private:
	void basicErrorCheck( size_t pos );
private:
	std::vector<BYTE> mBuffer;
};


#endif //GAMEBOY_CARTRIDGE_H
