//
// Created by HaruGakkaP on 2020-11-30.
//

#include "MBC1.h"

MBC1::MBC1(Cartridge &&cartridge) : mCartridge( cartridge )
{
}


BYTE MBC1::Get(size_t mem_addr) const
{



	return 0;
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
			setRomBankHigh3Bit( mem_addr, value );
			break;
		case 6: // BANK MODE SELECT
		case 7:
			setBankMode( mem_addr, value );
			break;
		default:

			break;
	}
}

void MBC1::setActiveRAM(size_t mem_addr, BYTE value)
{
	mRamEnable = value == 0xa;
}

void MBC1::setRomBankLeast5Bit(size_t mem_addr, BYTE value)
{
	value &= 0b00011111u;
	value = value == 0 ? 1 : value;

	mRomBankNumber = ( mRomBankNumber & 0b11100000u ) | ( value );
}

void MBC1::setRomBankHigh3Bit(size_t mem_addr, BYTE value)
{

}

void MBC1::setBankMode(size_t mem_addr, BYTE value)
{

}
