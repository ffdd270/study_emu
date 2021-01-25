//
// Created by HaruGakkaP on 2020-12-11.
//

#ifndef GAMEBOY_MEMORYMANAGEUNIT_H
#define GAMEBOY_MEMORYMANAGEUNIT_H

#include "memory/MemoryInterface.h"
#include <array>

class MemoryManageUnit final : public MemoryInterface
{
public:
	explicit MemoryManageUnit(
			std::shared_ptr<MemoryInterface> ptr_cartridge,
			std::shared_ptr<MemoryInterface> ptr_vram);

	[[nodiscard]] BYTE Get(size_t mem_addr) const override;
	void Set(size_t mem_addr, BYTE value) override;

	void SetCartridge( std::shared_ptr<MemoryInterface> ptr_cartridge );

	[[nodiscard]] bool IsReportedInterrupt() const override { return false; }
	void Reset() override {}
	void ResolveInterrupt(WORD resolve_interrupt_address) override { }
	~MemoryManageUnit() final = default;
private:

	//Hi - Ram.
	// 0xff80~0xfffe.
	std::array<BYTE, 0x7e> mHRAM;

	std::shared_ptr<MemoryInterface> mCartridge;
	std::shared_ptr<MemoryInterface> mVRAM;
};


#endif //GAMEBOY_MEMORYMANAGEUNIT_H
