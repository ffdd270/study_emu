//
// Created by HaruGakkaP on 2020-11-23.
//

#include "Cartridge.h"
#include <fstream>

void Cartridge::Load(std::string_view path)
{
	std::fstream file( path.data(), std::fstream::binary | std::fstream::in );
	if( !file.is_open() ) { throw std::logic_error("NO FILE");}

	std::vector<char> buffer( (std::istreambuf_iterator<char>( file )), std::istreambuf_iterator<char>()  );
	if( buffer.empty() ) { throw std::logic_error("NO DATA"); }
	// 검증


	// 버퍼
	mBuffer = std::move( buffer );
}
