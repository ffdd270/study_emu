//
// Created by HaruGakkaP on 2020-11-30.
//

#ifndef GAMEBOY_MBC1_H
#define GAMEBOY_MBC1_H

#include "Cartridge.h"

/*
 * Gameboy의 기본적인 Memory Controller.
 *
 *
 */
enum BankMode
{
	ROM, RAM
};

class MBC1
{
public:
	MBC1( Cartridge && cartridge );

	[[nodiscard]] BYTE Get(size_t mem_addr) const;
	void Set(size_t mem_addr, BYTE value);

	[[nodiscard]] bool IsRAMActive() const { return mRamEnable; }
	[[nodiscard]] BYTE GetRomBankNumber() const { return mSelectBank; }
	[[nodiscard]] BankMode GetBankMode() const {  return mBankMode; }
private:
	[[nodiscard]] BYTE getROMBank00( size_t mem_addr ) const;
	[[nodiscard]] BYTE getROMSelectedBank( size_t mem_addr ) const;
	[[nodiscard]] BYTE getRAMSelectedBank( size_t mem_addr ) const;

	void setActiveRAM( size_t mem_addr, BYTE value );
	void setRomBankLeast5Bit( size_t mem_addr, BYTE value );
	void setBankHigh2Bit(size_t mem_addr, BYTE value );
	void setBankMode( size_t mem_addr, BYTE value );
private:
	Cartridge mCartridge;
	BankMode mBankMode = BankMode::ROM;

	bool mRamEnable = false;
	BYTE mSelectBank = 1;
};


#endif //GAMEBOY_MBC1_H
