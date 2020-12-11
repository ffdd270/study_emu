//
// Created by HaruGakkaP on 2020-12-11.
//

#include "MemoryManageUnit.h"

BYTE MemoryManageUnit::Get(size_t mem_addr) const
{
	if( mem_addr < 0xff80u )
	{
		return mCartridge->Get( mem_addr );
	}
	else if( mem_addr >= 0xff80u && mem_addr <= 0xfffe )
	{
		return mHRAM[ mem_addr - 0xff80u ];
	}

	return 0;
}

void MemoryManageUnit::Set(size_t mem_addr, BYTE value)
{
	if( mem_addr < 0xff80u )
	{
		mCartridge->Set( mem_addr, value );
	}
	else if( mem_addr >= 0xff80u && mem_addr <= 0xfffe )
	{
		mHRAM[ mem_addr - 0xff80u ] = value;
	}
}

MemoryManageUnit::MemoryManageUnit(std::shared_ptr<MemoryInterface> ptr_cartridge)
{
	if ( ptr_cartridge == nullptr ) { throw std::logic_error("CARTRIDGE SHOULD NOT NULLPTR"); }

	mCartridge = std::move( ptr_cartridge );
}
