//
// Created by HaruGakkaP on 2020-11-30.
//

#ifndef GAMEBOY_MBC1_H
#define GAMEBOY_MBC1_H

#include "Cartridge.h"
#include "MemoryInterface.h"


/*
 * Gameboy의 기본적인 Memory Controller.
 *
 *
 */
enum class BankMode
{
	ROM, RAM
};

class MBC1 : public MemoryInterface
{
public:
	MBC1( Cartridge && cartridge );

	[[nodiscard]] BYTE Get(size_t mem_addr) const override;
	void Set(size_t mem_addr, BYTE value) override;

	[[nodiscard]] bool IsRAMActive() const { return mRamEnable; }
	[[nodiscard]] BYTE GetSelectBank() const { return mSelectBank; }
	[[nodiscard]] BankMode GetBankMode() const {  return mBankMode; }
private:
	[[nodiscard]] BYTE getSelectRomBank() const;
	[[nodiscard]] BYTE getSelectRamBank() const;

	[[nodiscard]] size_t getCartridgeRamAddress( size_t mem_address ) const;

	[[nodiscard]] BYTE getROMBank00( size_t mem_addr ) const;
	[[nodiscard]] BYTE getROMSelectedBank( size_t mem_addr ) const;
	[[nodiscard]] BYTE getRAMSelectedBank( size_t mem_addr ) const;

	void setActiveRAM( size_t mem_addr, BYTE value );
	void setRomBankLeast5Bit( size_t mem_addr, BYTE value );
	void setBankHigh2Bit(size_t mem_addr, BYTE value );
	void setBankMode( size_t mem_addr, BYTE value );
	void setRAM( size_t mem_addr, BYTE value );
private:
	Cartridge mCartridge;
	BankMode mBankMode = BankMode::ROM;

	bool mRamEnable = false;
	BYTE mSelectBank = 1;
};


#endif //GAMEBOY_MBC1_H
