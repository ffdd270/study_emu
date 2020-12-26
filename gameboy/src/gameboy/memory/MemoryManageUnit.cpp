//
// Created by HaruGakkaP on 2020-12-11.
//

#include "MemoryManageUnit.h"

MemoryManageUnit::MemoryManageUnit(std::shared_ptr<MemoryInterface> ptr_cartridge,
								   std::shared_ptr<MemoryInterface> ptr_vram)
{
	mCartridge = std::move( ptr_cartridge );
	mVRAM = std::move( ptr_vram );
}

BYTE MemoryManageUnit::Get(size_t mem_addr) const
{
	if( ( mem_addr <= 0x7fffu ) || ( mem_addr >= 0xA000u && mem_addr <= 0xBFFF ) ) // CARTRIDGE ROM || RAM
	{
		if ( mCartridge == nullptr ) { throw std::logic_error("NOT LOADED CARTRIDGE."); }
		return mCartridge->Get( mem_addr );
	}
	else if ( mem_addr >= 0x8000u && mem_addr <= 0x9fff ) // GPU
	{
		if ( mVRAM == nullptr ) { throw  std::logic_error("NOT LOADED GPU"); }
		return mVRAM->Get( mem_addr );
	}
	else if( mem_addr >= 0xff80u && mem_addr <= 0xfffe ) // HI RAM
	{
		return mHRAM[ mem_addr - 0xff80u ];
	}

	return 0;
}

void MemoryManageUnit::Set(size_t mem_addr, BYTE value)
{
	if( ( mem_addr <= 0x7fffu ) || ( mem_addr >= 0xA000u && mem_addr <= 0xBFFF ) ) // CARTRIDGE ROM || RAM
	{
		if ( mCartridge == nullptr ) { throw std::logic_error("NOT LOADED CARTRIDGE."); }
		mCartridge->Set( mem_addr, value );
	}
	else if ( mem_addr >= 0x8000u && mem_addr <= 0x9fff || ( mem_addr >= 0xff00 && mem_addr <= 0xff7f ) ) // GPU
	{
		if ( mVRAM == nullptr ) { throw  std::logic_error("NOT LOADED GPU"); }
		return mVRAM->Set( mem_addr, value );
	}
	else if( mem_addr >= 0xff80u && mem_addr <= 0xfffe )
	{
		mHRAM[ mem_addr - 0xff80u ] = value;
	}
}

void MemoryManageUnit::SetCartridge(std::shared_ptr<MemoryInterface> ptr_cartridge)
{
	mCartridge = std::move( ptr_cartridge );
}
