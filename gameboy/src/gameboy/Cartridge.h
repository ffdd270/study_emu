//
// Created by HaruGakkaP on 2020-11-23.
//

#ifndef GAMEBOY_CARTRIDGE_H
#define GAMEBOY_CARTRIDGE_H

#include "typedef.h"
#include <vector>
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

	std::string GetTitle();
	ColorGameBoyFlag GetCGBFlag();
	BYTE GetCartridgeType();
	CartridgeSizeInfo GetSizeInfo();
	size_t GetRealBufferSize() { return mBuffer.size(); }
private:
	void basicErrorCheck( size_t pos );
private:
	std::vector<BYTE> mBuffer;
};


#endif //GAMEBOY_CARTRIDGE_H
