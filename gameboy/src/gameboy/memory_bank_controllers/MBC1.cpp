//
// Created by HaruGakkaP on 2020-11-30.
//

#include "MBC1.h"

MBC1::MBC1(Cartridge &&cartridge) : mCartridge( cartridge )
{
}


BYTE MBC1::Get(size_t mem_addr)
{



	return 0;
}


void MBC1::Set(size_t mem_addr)
{
	size_t hi = ( mem_addr & 0xf000u ) >> 12u;

	switch ( hi )
	{
		case 0: // RAM 활성화
			setActiveRAM( mem_addr );
			break;
		case 2: // ROM 뱅크 번호
			setRomBankLeast5Bit( mem_addr );
			break;
		case 4: // RAM 뱅크 번호, ROM 뱅크 번호
			setRomBankHigh3Bit( mem_addr );
			break;
		case 6: // BANK MODE SELECT
			setBankMode( mem_addr );
			break;
		default:

			break;
	}
}

void MBC1::setActiveRAM(size_t mem_addr)
{

}

void MBC1::setRomBankLeast5Bit(size_t mem_addr)
{

}

void MBC1::setRomBankHigh3Bit(size_t mem_addr)
{

}

void MBC1::setBankMode(size_t mem_addr)
{

}
