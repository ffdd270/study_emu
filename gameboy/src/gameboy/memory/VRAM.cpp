//
// Created by HaruGakkaP on 2020-12-15.
//

#include "VRAM.h"


VRAM::VRAM() : mMemory( { 0 } )
{

}

constexpr size_t VRAM_START_ADDRESS = 0x8000;
// 0x8000~0x9fff
BYTE VRAM::Get(size_t mem_addr) const
{
	checkAddress(mem_addr);
	return mMemory[mem_addr];
}


void VRAM::Set(size_t mem_addr, BYTE value)
{
	checkAddress(mem_addr);
	mMemory[mem_addr] = value;
}

void VRAM::checkAddress(size_t mem_addr) const
{
	if( mem_addr >= mMemory.size() ) { throw  std::logic_error("WRONG ADDRESS."); }
}


