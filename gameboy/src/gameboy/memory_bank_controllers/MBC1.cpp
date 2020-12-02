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


BYTE MBC1::getSelectRomBank() const
{
	return mSelectBank == BankMode::ROM ?
		mSelectBank & 0x1fu :
		mSelectBank & 0x7fu ;
}

BYTE MBC1::getSelectRamBank() const
{
	return 0;
}

BYTE MBC1::getROMBank00(size_t mem_addr) const
{
	return mCartridge.GetData( mem_addr );
}

BYTE MBC1::getROMSelectedBank(size_t mem_addr) const
{
	return mCartridge.GetData( ( getSelectRomBank() * 0x4000 ) + ( mem_addr - 0x4000 )  );
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

/*
 * 모드 0에서 2 비트 보조 뱅킹 레지스터는 4000-7FFF 뱅크 ROM 영역에만 영향을 줄 수 있습니다. 카트리지가 "소형 ROM" / "대형 RAM"카트 (<1MB ROM,> 8kB RAM) 인 경우,
 * 4000-7FFF는 어쨌든 이 레지스터의 영향을 받지 않으므로 실제 효과는 RAM 뱅킹이 비활성화되고 A000-BFFF입니다.
 * RAM의 뱅크 0에만 액세스 할 수 있도록 잠겨 있으며 2 비트 보조 뱅킹 레지스터는 완전히 무시됩니다.
 *
 * 모드 1에서는 현재 카트가 "대형 RAM"카트 (> 8kB RAM)인지 "대형 ROM"카트 (1MB 이상)인지에 따라 동작이 다릅니다.
 * 대형 RAM 카트의 경우 모드 1로 전환하면 RAM 뱅킹이 활성화되고 (RAM이 활성화 된 경우)
 * 즉시 A000-BFFF RAM 영역을 2 비트 보조 뱅킹 레지스터에서 선택한 뱅크로 전환합니다.
 */
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
