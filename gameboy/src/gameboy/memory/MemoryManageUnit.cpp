//
// Created by HaruGakkaP on 2020-12-11.
//

#include "MemoryManageUnit.h"

#include <utility>
#include <stdexcept>

MemoryManageUnit::MemoryManageUnit(std::shared_ptr<MemoryInterface> ptr_cartridge,
                                   std::shared_ptr<MemoryInterface> ptr_vram,
								   std::shared_ptr<MemoryInterface> ptr_timer)
{
	mCartridge = std::move(ptr_cartridge);
	mVRAM = std::move( ptr_vram );
	mTimer = std::move(ptr_timer);


	mBankNum = 0;
	mInterruptEnable = 0;
	mInterruptFlags = 0;
}

BYTE MemoryManageUnit::Get(size_t mem_addr) const
{
	if( ( mem_addr <= 0x7fffu ) || ( mem_addr >= 0xA000u && mem_addr <= 0xBFFF ) ) // CARTRIDGE ROM || RAM
	{
		if ( mCartridge == nullptr ) { throw std::logic_error("NOT LOADED CARTRIDGE."); }
		return mCartridge->Get( mem_addr );
	}
	// GPU
	else if ( mem_addr >= 0x8000u && mem_addr <= 0x9fff || // VRAM
			  ( mem_addr >= 0xfe00 && mem_addr <= 0xfe9f ) || // OAM
			  ( mem_addr >= 0xff40 && mem_addr <= 0xff7f ) ) // Interrupts
	{
		if ( mVRAM == nullptr ) { throw  std::logic_error("NOT LOADED GPU"); }
		return mVRAM->Get( mem_addr );
	}
	// Work Ram BANK 0
	else if ( mem_addr >= 0xC000u && mem_addr <= 0xCFFF )
	{
		return mWorkRam[ ( mem_addr - 0xC000u ) ];
	}
	else if ( mem_addr >= 0xD000u && mem_addr <= 0xDFFF )
	{
		return mWorkRam[ ( mBankNum * 0x1000u ) + ( mem_addr - 0xD000u ) ];
	}
	else if( mem_addr >= 0xff04u && mem_addr <= 0xff07u )
	{
		if (mTimer == nullptr) { throw std::logic_error("NOT LOADED TIMER"); }
		return mTimer->Get( mem_addr );

	}
	else if( mem_addr >= 0xff80u && mem_addr <= 0xfffe ) // HI RAM
	{
		return mHRAM[ mem_addr - 0xff80u ];
	}
	else if( mem_addr == 0xff0f )
	{
		return mInterruptFlags;
	}
	else if ( mem_addr == 0xffff )
	{
		return mInterruptEnable;
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
	// GPU
	else if ( mem_addr >= 0x8000u && mem_addr <= 0x9fff || // VRAM
			( mem_addr >= 0xfe00 && mem_addr <= 0xfe9f ) || // OAM
			( mem_addr >= 0xff40 && mem_addr <= 0xff7f ) ) // Interrupts
	{
		if ( mVRAM == nullptr ) { throw  std::logic_error("NOT LOADED GPU"); }
		return mVRAM->Set( mem_addr, value );
	}
		// Work Ram BANK 0
	else if ( mem_addr >= 0xC000u && mem_addr <= 0xCFFF )
	{
		mWorkRam[ ( mem_addr - 0xC000u ) ] = value;
	}
	else if ( mem_addr >= 0xD000u && mem_addr <= 0xDFFF )
	{
		mWorkRam[ ( mBankNum * 0x1000u ) + ( mem_addr - 0xD000u ) ] = value;
	}
	else if( mem_addr >= 0xff04u && mem_addr <= 0xff07u )
	{
		mTimer->Set( mem_addr, value );
	}
	else if( mem_addr >= 0xff80u && mem_addr <= 0xfffe )
	{
		mHRAM[ mem_addr - 0xff80u ] = value;
	}
	else if( mem_addr == 0xff0f )
	{
		mInterruptFlags = value & 0x1fu; // 5비트만 씀
	}
	else if ( mem_addr == 0xffff )
	{
		mInterruptEnable = value & 0x1fu; // 5비트만 씀
	}
}

void MemoryManageUnit::SetCartridge(std::shared_ptr<MemoryInterface> ptr_cartridge)
{
	mCartridge = std::move(ptr_cartridge);
}
