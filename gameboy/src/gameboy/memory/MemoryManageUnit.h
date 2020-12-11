//
// Created by HaruGakkaP on 2020-12-11.
//

#ifndef GAMEBOY_MEMORYMANAGEUNIT_H
#define GAMEBOY_MEMORYMANAGEUNIT_H

#include "memory/MemoryInterface.h"
#include <array>

class MemoryManageUnit : public MemoryInterface
{
public:
	explicit MemoryManageUnit( std::shared_ptr<MemoryInterface> ref_ptr_cartridge );

	[[nodiscard]] BYTE Get(size_t mem_addr) const override;
	void Set(size_t mem_addr, BYTE value) override;
private:

	//Hi - Ram.
	// 0xff80~0xfffe.
	std::array<BYTE, 0x7e> mHRAM;

	std::shared_ptr<MemoryInterface> mCartridge;
};


#endif //GAMEBOY_MEMORYMANAGEUNIT_H
