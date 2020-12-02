//
// Created by HaruGakkaP on 2020-11-30.
//

#include "MBC1.h"

MBC1::MBC1(Cartridge &&cartridge) : mCartridge( cartridge )
{
}


BYTE MBC1::Get(size_t mem_addr) const
{
	size_t hi = ( mem_addr & 0xf000u ) >> 12u;

	BYTE result = 0x0;

	switch ( hi )
	{
		case 0:
		case 1:
		case 2:
		case 3:
			result = getROMBank00( mem_addr );
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			result = getROMSelectedBank( mem_addr );
			break;
		case 0xA:
		case 0xB:
			result = getRAMSelectedBank( mem_addr );
			break;
		default:
			// INVALID ACCESS
			break;
	}

	return result;
}


void MBC1::Set(size_t mem_addr, BYTE value)
{
	size_t hi = ( mem_addr & 0xf000u ) >> 12u;

	switch ( hi )
	{
		case 0: // RAM 활성화
		case 1:
			setActiveRAM( mem_addr, value );
			break;
		case 2: // ROM 뱅크 번호
		case 3:
			setRomBankLeast5Bit( mem_addr, value );
			break;
		case 4: // RAM 뱅크 번호, ROM 뱅크 번호
		case 5:
			setBankHigh2Bit(mem_addr, value);
			break;
		case 6: // BANK MODE SELECT
		case 7:
			setBankMode( mem_addr, value );
			break;
		default:

			break;
	}
}

BYTE MBC1::getROMBank00(size_t mem_addr) const
{
	return mCartridge.GetData( mem_addr );
}

BYTE MBC1::getROMSelectedBank(size_t mem_addr) const
{
	return 0;
}

BYTE MBC1::getRAMSelectedBank(size_t mem_addr) const
{
	return 0;
}


void MBC1::setActiveRAM(size_t mem_addr, BYTE value)
{
	mRamEnable = value == 0xa;
}

void MBC1::setRomBankLeast5Bit(size_t mem_addr, BYTE value)
{
	value &= 0b00011111u;
	value = value == 0 ? 1 : value;

	mSelectBank = ( mSelectBank & 0b11100000u ) | ( value );
}

// bank는 ROM. RAM 둘 다 쓰고. 상위 2비트에 기록한다.
// 이걸 구분하는 건 Bank Mode에서.
void MBC1::setBankHigh2Bit(size_t mem_addr, BYTE value)
{
	value &= 0b00000011u; // 2비트 받고.
	value <<= 5u; // 5비트 쉬프트 Left..

	mSelectBank = ( mSelectBank & 0b00011111u ) | ( value );
}

void MBC1::setBankMode(size_t mem_addr, BYTE value)
{
	switch ( value )
	{
		case 0:
			mBankMode = BankMode::ROM;
			break;
		case 1:
			mBankMode = BankMode::RAM;
			break;
		default:
			throw std::logic_error("INVALID BANK MODE : IN MBC1!");
	}
}
