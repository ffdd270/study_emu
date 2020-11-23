//
// Created by HaruGakkaP on 2020-11-23.
//

#ifndef GAMEBOY_CARTRIDGE_H
#define GAMEBOY_CARTRIDGE_H

#include <string_view>

class Cartridge
{
public:
	Cartridge();

	void Load( std::string_view path );
private:
};


#endif //GAMEBOY_CARTRIDGE_H
