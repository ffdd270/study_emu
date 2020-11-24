//
// Created by HaruGakkaP on 2020-11-23.
//

#ifndef GAMEBOY_CARTRIDGE_H
#define GAMEBOY_CARTRIDGE_H

#include <vector>
#include <string_view>

class Cartridge
{
public:
	Cartridge() = default;

	void Load( std::string_view path );
	std::string GetTitle();

private:
	std::vector<char> mBuffer;
};


#endif //GAMEBOY_CARTRIDGE_H
