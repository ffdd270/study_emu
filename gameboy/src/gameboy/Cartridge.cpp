//
// Created by HaruGakkaP on 2020-11-23.
//

#include "Cartridge.h"
#include <fstream>
#include <algorithm>

void Cartridge::Load(std::string_view path)
{
	std::fstream file( path.data(), std::fstream::binary | std::fstream::in );
	if( !file.is_open() ) { throw std::logic_error("NO FILE");}

	std::vector<BYTE> buffer( (std::istreambuf_iterator<char>( file )), std::istreambuf_iterator<char>()  );
	if( buffer.empty() ) { throw std::logic_error("NO DATA"); }
	// 검증

	// 버퍼
	mBuffer = std::move( buffer );

	//RAM 할당
	mRam.resize( GetRamSizeInfo().size );
}

std::vector<BYTE> Cartridge::GetRawCartridgeData() const
{
	return mBuffer;
}

BYTE Cartridge::GetData( size_t mem_pos ) const
{
	basicErrorCheck( mem_pos );
	return mBuffer[mem_pos];
}

BYTE Cartridge::GetRamData(size_t mem_pos) const
{
	ramErrorCheck(mem_pos);
	return mRam[mem_pos];
}

std::string Cartridge::GetTitle() const
{
	const size_t TITLE_START_POINT = 0x134;
	const size_t TITLE_END_POINT = 0x143;

	basicErrorCheck( TITLE_END_POINT );

	std::string name = std::string( &mBuffer[TITLE_START_POINT], &mBuffer[TITLE_END_POINT] );

	// Null 문자 제거
	name.erase( std::remove_if( name.begin(), name.end(), []( unsigned char c ) -> bool {
		return c == '\x00';
	} ), name.end() );

	return name;
}

Cartridge::ColorGameBoyFlag Cartridge::GetCGBFlag() const
{
	constexpr size_t CGB_FLAG_POINT = 0x143;
	basicErrorCheck( CGB_FLAG_POINT );

	auto value = mBuffer[ CGB_FLAG_POINT ];
	auto rtn = ColorGameBoyFlag::CGB_UNKNOWN;

	switch ( value )
	{
		case 0x00:
			rtn = ColorGameBoyFlag::CGB_NONE;
			break;
		case 0x80:
			rtn = ColorGameBoyFlag::CGB_SUPPORT;
			break;
		case 0xC0:
			rtn = ColorGameBoyFlag::CGB_ONLY;
			break;
		default:
			break;
	}

	return rtn;
}

BYTE Cartridge::GetCartridgeType() const
{
	constexpr size_t CARTRIDGE_TYPE_POINT = 0x147;
	basicErrorCheck( CARTRIDGE_TYPE_POINT );

	return mBuffer[ CARTRIDGE_TYPE_POINT ];
}

constexpr size_t K = 1024;

CartridgeSizeInfo Cartridge::GetRamSizeInfo() const
{
	constexpr size_t SIZE_INFOS[] = {
			0,
			2 * K,
			8 * K,
			32 * K,
			128 * K,
			64 * K,
	};

	constexpr size_t BANK_INFOS[] = {
			1,
			1,
			1,
			4,
			16,
			8,
	};

	constexpr size_t RAM_SIZE_POINT = 0x149;
	basicErrorCheck( RAM_SIZE_POINT );

	auto value = mBuffer[RAM_SIZE_POINT];
	if ( value > 5 ) { throw std::logic_error("Ram Size info Out of index.");}

	CartridgeSizeInfo info;
	info.size = SIZE_INFOS[ value ];
	info.bank = BANK_INFOS[ value ];

	return info;
}


CartridgeSizeInfo Cartridge::GetSizeInfo() const
{
	constexpr size_t SIZE_BASIC_INFOS [] = {
			32 * K,
			64 * K,
			128 * K,
			256 * K,
			512 * K,
			( 1 * K  )* K, // MB
			( 2 * K ) * K,
			( 4 * K ) * K,
			( 8 * K ) * K
	};

	constexpr size_t BANK_BASIC_INFOS [] = {
			2, 4, 8, 16, 32, 64, 128, 256, 512
	};

	constexpr size_t SIZE_EXTEND_INFOS [] = {
			0, 0,
			static_cast<size_t>( 1.1 * K ) * K,
			static_cast<size_t>( 1.2 * K ) * K,
			static_cast<size_t>( 1.5 * K ) * K,
	};

	constexpr size_t BANK_EXTEND_INFOS [] = {
			0, 0, 72, 80, 96
	};

	constexpr size_t CARTRIDGE_TYPE_POINT = 0x147;
	basicErrorCheck( CARTRIDGE_TYPE_POINT );

	auto value = mBuffer[CARTRIDGE_TYPE_POINT];
	BYTE hi = (value & 0xf0u) >> 4u;
	BYTE lo = (value & 0x0fu);

	if( lo > 8 || ( hi == 5 ) && ( lo > 4 ) ) { throw  std::logic_error("WRONG SIZE INFO."); }

	size_t size = hi == 5 ? SIZE_EXTEND_INFOS[lo] : SIZE_BASIC_INFOS[lo];
	size_t bank = hi == 5 ? BANK_EXTEND_INFOS[lo] : BANK_BASIC_INFOS[lo];

	if( size == 0 || bank == 0 ) { throw std::logic_error("HI = 5, But Access to 1~2"); }

	CartridgeSizeInfo info;
	info.size = size;
	info.bank = bank;

	return info;
}

std::array<BYTE, 4> Cartridge::GetEntryPoint() const
{
	constexpr size_t ENTRY_POINT = 0x100;
	constexpr size_t READ_BYTE = 0x4;
	basicErrorCheck( ENTRY_POINT + READ_BYTE  );

	std::array<BYTE, 4> entry_data = { 0 };

	for( size_t data_index = 0; data_index < READ_BYTE; data_index++ )
	{
		entry_data[data_index] = mBuffer[ENTRY_POINT + data_index];
	}

	return entry_data;
}

void Cartridge::ramErrorCheck(size_t pos) const
{
	if ( mRam.empty() ) { throw std::logic_error("Not Have RAM."); }
	if ( mRam.size() <= pos ) { throw std::logic_error("Out of Index, RAM!"); }
}

void Cartridge::basicErrorCheck(const size_t pos) const
{
	if( mBuffer.empty() ) { throw std::logic_error("Cartridge Not INITED."); }
	if( mBuffer.size() <= pos ) { throw std::logic_error("NOT VALID CARTRIDGE."); }
}
