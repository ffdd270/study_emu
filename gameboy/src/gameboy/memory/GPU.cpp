//
// Created by HaruGakkaP on 2020-12-15.
//

#include "GPU.h"


GPU::GPU() : mMemory({0 } )
{

}

constexpr size_t VRAM_START_ADDRESS = 0x8000;
// 0x8000~0x9fff
BYTE GPU::Get(size_t mem_addr) const
{
	checkAddress(mem_addr);
	return mMemory[mem_addr - VRAM_START_ADDRESS];
}


void GPU::Set(size_t mem_addr, BYTE value)
{
	checkAddress(mem_addr);
	mMemory[mem_addr - VRAM_START_ADDRESS] = value;
}

void GPU::checkAddress(size_t mem_addr) const
{
	int result_relative_address = static_cast<int>( mem_addr ) - static_cast<int>( VRAM_START_ADDRESS );

	if (result_relative_address < 0 ) { throw  std::logic_error("UNDERFLOW, ADDRESS"); }
	if (result_relative_address >= mMemory.size()) { throw std::logic_error("OVERFLOW, ADDRESS."); }

}


