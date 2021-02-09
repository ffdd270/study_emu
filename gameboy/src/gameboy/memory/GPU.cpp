//
// Created by HaruGakkaP on 2020-12-15.
//

#include <string>
#include <vector>
#include "GPU.h"


inline bool GetBit( BYTE origin, BYTE bit_pos )
{
	return ( origin & ( 0b1u << bit_pos ) ) >> bit_pos;
}

inline void SetBit( BYTE & origin, BYTE bit_pos )
{
	origin = ( origin | ( 1u << bit_pos ) );
}

inline void OffBit( BYTE & origin, BYTE bit_pos )
{
	origin = origin & ( 0xFFu ^ ( 0b1u << bit_pos ) );
}


bool GPUHelper::IsLCDDisplayEnable(BYTE value)
{
	return GetBit( value, 7 ) == 1;
}
WORD GPUHelper::GetSelectedWindowTileMap(BYTE value)
{
	return GetBit( value, 6 ) == 1 ?
		   0x9C00u :
		   0x9800u ;
}


bool GPUHelper::IsWindowDisplayEnable(BYTE value)
{
	return GetBit( value, 5 ) == 1;
}

WORD GPUHelper::GetSelectBGAndWindowTileData(BYTE value)
{
	return GetBit( value, 4 ) == 1 ?
		   0x8000u :
		   0x8800u ;
}

WORD GPUHelper::GetSelectBGTileMapDisplay(BYTE value)
{
	return GetBit( value, 3 ) == 1 ?
		   0x9C00u :
		   0x9800u ;
}
bool GPUHelper::IsSpriteSize(BYTE value)
{
	return GetBit( value, 2 ) == 1;
}

bool GPUHelper::IsSpriteDisplayEnable(BYTE value)
{
	return GetBit( value, 1 ) == 1;
}

bool GPUHelper::CheckProperty(BYTE value)
{
	return GetBit( value, 0 ) == 1;
}

//LCD Status Register

bool GPUHelper::IsEnableLYCoincidenceInterrupt(BYTE value)
{
	return GetBit( value, 6 ) == 1;
}

bool GPUHelper::IsEnableMode2OAMInterrupt(BYTE value)
{
	return GetBit( value, 5 ) == 1;
}


bool GPUHelper::IsEnableMode1VBlankInterrupt(BYTE value)
{
	return GetBit( value, 4 ) == 1;
}

bool GPUHelper::IsEnableMode0HBlankInterrupt(BYTE value)
{
	return GetBit( value, 3 ) == 1;
}

bool GPUHelper::IsCoincidence(BYTE value)
{
	return GetBit( value, 2 ) == 1;
}

BYTE GPUHelper::GetModeFlag(BYTE value)
{
	BYTE bit1 = GetBit( value, 1 );
	BYTE bit0 = GetBit( value, 0 );

	return static_cast<BYTE>( bit1 << 1u ) | bit0 ;
}

GPUHelper::MonoPallet GPUHelper::GetPalletData(BYTE pallet_data, size_t pos)
{
 	BYTE result = pallet_data & 0b11u << ( pos * 2 );
	return static_cast<GPUHelper::MonoPallet>( result >> pos * 2 );
}

// 거꾸로 뒤집혀 있다.
std::array<BYTE, 8> GPUHelper::ToTileData(BYTE lo, BYTE hi)
{
	std::array<BYTE, TileDataWidth> tile = {};

	for( size_t i = 0; i < TileDataWidth; i++ )
	{
		BYTE lo_bit = GetBit( lo, 7 - i );
		BYTE hi_bit = GetBit( hi, 7 - i );

		tile[i] = static_cast<BYTE>( hi_bit << 1u ) | lo_bit;
	}

	return tile;
}

GPU::GPU() :
	mLCDStatusRegister( 0 ), mLCDControlRegister( 0 ),
		mDots( 0 ), mScanLineY( 0 ),
		mScrollX( 0 ), mScrollY( 0 ),
		mLYC( 0 ), mBGColorPalletIndex( 0 ), mObjectColorPalletIndex( 0 ),
		mHDMASourceHi( 0 ), mHDMASourceLo( 0 ),
		mHDMADestHi( 0 ), mHDMADestLo(0 ),
		mHDMAStatus( 0 ), mIsHDMAStart(false ),
		mDMASourceHi( 0 ), mIsDMAStart( false ),
		mSelectVRAMBank(0 ), mReportLCDStat( false ), mReportVBlank( false )
{
	for(ColorScreenLine & line : mColorScreen )
	{
		for( GPUHelper::ColorPallet & pallet : line )
		{
			pallet.SetLo( 0x7f );
			pallet.SetHi( 0x7f );
		}
	}

	for(MonoScreenLine & line : mMonoScreen )
	{
		for( GPUHelper::MonoPallet & pallet : line )
		{
			pallet = GPUHelper::MonoPallet::BLACK;
		}
	}


}

constexpr size_t VRAM_START_ADDRESS = 0x8000;
constexpr size_t VRAM_END_ADDRESS = 0x9fff;
// 0x8000~0x9fff

constexpr size_t OAM_START_ADDRESS = 0xfe00;
constexpr size_t OAM_END_ADDRESS = 0xfe9f;

BYTE GPU::Get(size_t mem_addr) const
{
	if( mem_addr >= VRAM_START_ADDRESS && mem_addr <= VRAM_END_ADDRESS ) // VRAM
	{
		checkAddress(mem_addr);
		return mMemory[mSelectVRAMBank][mem_addr - VRAM_START_ADDRESS];
	}
	else if( mem_addr >= OAM_START_ADDRESS && mem_addr <= OAM_END_ADDRESS )
	{
		return mObjectAttributeMemory[mem_addr - OAM_START_ADDRESS];
	}
	else
	{
		return procInterruptsOnGet( mem_addr );
	}
}


void GPU::Set(size_t mem_addr, BYTE value)
{
	if( mem_addr >= VRAM_START_ADDRESS && mem_addr <= VRAM_END_ADDRESS )
	{
		checkAddress(mem_addr);
		mMemory[mSelectVRAMBank][mem_addr - VRAM_START_ADDRESS] = value;
	}
	else if( mem_addr >= OAM_START_ADDRESS && mem_addr <= OAM_END_ADDRESS )
	{
		mObjectAttributeMemory[mem_addr - OAM_START_ADDRESS] = value;
	}
	else
	{
		procInterruptsOnSet( mem_addr, value );
	}
}

bool GPU::IsReportedInterrupt() const
{
	return mIsHDMAStart || mIsDMAStart || mReportVBlank || mReportLCDStat;
}

std::vector<InterruptsType> GPU::GetReportedInterrupts() const
{
	std::vector<InterruptsType> types;

	if ( mIsDMAStart )
	{
		types.emplace_back( InterruptsType::DMA );
	}
	else if ( mIsHDMAStart )
	{
		types.emplace_back( InterruptsType::HDMA );
	}
	else if ( mReportLCDStat )
	{
		types.emplace_back( InterruptsType::LCD_STAT );
	}
	else if ( mReportVBlank )
	{
		types.emplace_back( InterruptsType::V_BLANK );
	}

	return types;
}


void GPU::ResolveInterrupt(InterruptsType resolve_interrupt_address)
{
	switch (resolve_interrupt_address)
	{
		case InterruptsType::HDMA:
			mIsHDMAStart = false;
			break;
		case InterruptsType::DMA:
			mIsDMAStart = false;
			break;
		case InterruptsType::LCD_STAT:
			mReportLCDStat = false;
			break;
		case InterruptsType::V_BLANK:
			mReportVBlank = false;
			break;
		default:
			break;
	}
}



void GPU::NextStep(size_t clock)
{
	disableHBlank();

	// 1 Cycle = 4 Clock, 114 Cycle = 456 Dots = One Line.
	// Line == LY

	if (clock == 0)
	{
		return;
	}

	// 모드는 3개가 있다.
	// 모드 2 -> 서칭 오브젝트. 0~80 Dots.
	// 모드 3 -> 드로잉. 우리는 OpenGL로 한번에 플러시 때릴 거다. 그래서 할 게 없지만 Clock는 지켜줘야 한다. 172 고정 Dots.
	// 모드 0- > H-BLANK, 204 클럭. 처음으로 진입하면 여기서 OpenGL로 그릴 거다.

	// LY가 144가 넘어가면 v-blank가 올라간다.

	int loop_target = (static_cast<int>( clock ) / 80 ) + 1; // 왜 80인가? 라고 물으면..
	// 현대 컴퓨터에서 이걸 일일히 다 따라하기보단, 그냥 모드 전환만 적당히 되면 되서 그런 거 아닐까?
	// 그럼 더 큰값은 안 되냐고 생각할 지도 모르겠지만. 모드 2의 유지 기간이 80이라서. 80으로 해야 모든 모드들에 진입할 수 있다.


	for( int i = 0; i < loop_target; i++ )
	{
		if ( i == (loop_target - 1 ) ) // 마지막이면 80의 나머지 숫자를.
		{
			mDots += clock % 80;
		}
		else // 아니라면 그냥 몫만큼 더하면 됨.
		{
			mDots += 80;
		}

		size_t prv_dots = mDots;
		mDots = mDots % GPUHelper::LinePerDots;

		if ( prv_dots != mDots ) // 이게 다르다는 게 무슨 뜻이냐면, 라인이 넘어갔다는 뜻이다.
		{
			mScanLineY = ( mScanLineY + 1 ) % GPUHelper::MaxScanline; // 스캔라인은 154까지 있다.

			if ( IsEnableLYCoincidenceInterrupt() ) // 플래그 올라가면 체크.
			{
				setCoincidenceInterrupt(mScanLineY == mLYC); // LYC랑 같으면 인터럽트 발생.
				mReportLCDStat = true;
			}
		}

		if ( mScanLineY >= GPUHelper::RealScanlineEnd ) //  V-BLANK 이벤트. 이미 스캔라인은 다 그렸지만, CPU들이 VRAM에 접근할 수 있도록
		{
			if( GetModeFlag() == 1 )
			{
				continue; // 계속.
			}

			setLCDMode(1);
			enableVBlank();

			if (IsEnableMode1VBlankInterrupt())
			{
				mReportLCDStat = true;
			}

			mReportVBlank = true;
		}
		else if ( mDots <= 80 ) // Searching Object / OAM 접근 불가.
		{
			if ( GetModeFlag() == 2 )
			{
				continue;
			}

			if (IsEnableMode2OAMInterrupt())
			{
				mReportLCDStat = true;
			}

			setLCDMode( 2 );
		}
		else if( mDots <= 80 + 172 ) // Drawing, VRAM / OAM 접근 불가.
		{
			setLCDMode( 3 );
		}
		else if( mDots > 80 + 172 ) // HBLANK
		{
			if ( GetModeFlag() == 0  )
			{
				continue; // 계속.
			}

			setLCDMode( 0 );
			enableHBlank();

			if (IsEnableMode0HBlankInterrupt())
			{
				mReportLCDStat = true;
			}

			// TODO : 이제 실제로 그리면 됨.
			drawBackground();
			drawSprites();
		}
	}
}


//LCD Control Register

bool GPU::IsLCDDisplayEnable() const
{
	return GPUHelper::IsLCDDisplayEnable(mLCDControlRegister );
}

WORD GPU::GetSelectedWindowTileMap() const
{
	return GPUHelper::GetSelectedWindowTileMap(mLCDControlRegister );
}

bool GPU::IsWindowDisplayEnable() const
{
	return GPUHelper::IsWindowDisplayEnable(mLCDControlRegister );
}

WORD GPU::GetSelectBGAndWindowTileData() const
{
	return GPUHelper::GetSelectBGAndWindowTileData(mLCDControlRegister );
}

WORD GPU::GetSelectBGTileMapDisplay() const
{
	return GPUHelper::GetSelectBGTileMapDisplay(mLCDControlRegister );
}

bool GPU::IsSpriteSize() const
{
	return GPUHelper::IsSpriteSize(mLCDControlRegister );
}

bool GPU::IsSpriteDisplayEnable() const
{
	return GPUHelper::IsSpriteDisplayEnable(mLCDControlRegister );
}

bool GPU::CheckProperty() const
{
	return GPUHelper::CheckProperty(mLCDControlRegister );
}

//LCD Status Register

bool GPU::IsEnableLYCoincidenceInterrupt() const
{
	return GPUHelper::IsEnableLYCoincidenceInterrupt(mLCDStatusRegister );
}

bool GPU::IsEnableMode2OAMInterrupt() const
{
	return GPUHelper::IsEnableMode2OAMInterrupt(mLCDStatusRegister );
}

bool GPU::IsEnableMode1VBlankInterrupt() const
{
	return GPUHelper::IsEnableMode1VBlankInterrupt(mLCDStatusRegister );
}

bool GPU::IsEnableMode0HBlankInterrupt() const
{
	return GPUHelper::IsEnableMode0HBlankInterrupt(mLCDStatusRegister );
}

bool GPU::IsCoincidence() const
{
	return GPUHelper::IsCoincidence(mLCDStatusRegister);
}

BYTE GPU::GetModeFlag() const
{
	return GPUHelper::GetModeFlag(mLCDStatusRegister );
}

WORD GPU::GetDMASource() const
{
	return ( static_cast<WORD>(mDMASourceHi) << 8u );
}


WORD GPU::GetHDMASource() const
{
	WORD addr = static_cast<WORD>( static_cast<WORD>(mHDMASourceHi) << 8u ) | mHDMASourceLo;
	return( addr & 0xfff0u ); // 하위 4비트 버림.
}

WORD GPU::GetHDMADest() const
{
	WORD addr = static_cast<WORD>( static_cast<WORD>(mHDMADestHi) << 8u ) | mHDMADestLo;
	return( ( addr & 0x1ff0u ) + 0x8000 ); // 상위 3비트, 하위 4비트 버림. 무조건 0x8000번 기준임.
}

BYTE GPU::GetRemainHDMA() const
{
	return mHDMAStatus & 0x7fu;
}

BYTE GPU::GetHDMAMode() const
{
	return ( mHDMAStatus & 0x80u ) >> 7u ;
}

const MonoScreenBits *GPU::GetMonoScreenData() const
{
	return &mMonoScreen;
}

const ColorScreenBits *GPU::GetColorScreenData() const
{
	return &mColorScreen;
}

WORD GPU::GetSelectedTileAddress(BYTE tile_index) const
{
	WORD start_address = GetSelectBGAndWindowTileData();
	WORD tile_index_word = tile_index;

	return start_address + ( tile_index_word * 16 );
}

GPUHelper::ObjectAttribute GPU::GetObjectAttribute(BYTE oam_table_index) const
{
	size_t real_position = oam_table_index * 4;

	if ( ( real_position ) > ( mObjectAttributeMemory.size() - 1 )  )
	{
		throw std::logic_error("Object Attribute Overflowed.");
	}


	GPUHelper::ObjectAttribute result{};
	for( int i = 0; i < 4; i++ )
	{
		result.data[i] = mObjectAttributeMemory[real_position + i];
	}

	return result;
}


void GPU::SetHDMAAddresses(WORD source, WORD dest)
{
	mHDMASourceHi = ( source & 0xff00u ) >> 8u;
	mHDMASourceLo = ( source & 0x00f0u ); // 하위 비트는 무시한다.

	mHDMADestHi = ( dest & 0x1f00u ) >> 8u; // 상위 3비트는 무시된다.
	mHDMADestLo = ( dest & 0x00f0u ); // 하위 비트는 무시한다.
}

void GPU::SetRemainHDMA(BYTE remain)
{
	mHDMAStatus = ( mHDMAStatus & 0x80u ) | ( remain & 0x7fu );
}


void GPU::procInterruptsOnSet( size_t mem_addr, BYTE value )
{
	switch ( mem_addr )
	{
		case 0xff40: // LCD Control Register
		{
			mLCDControlRegister = value;
			break;
		}
		case 0xff41: // LCD Status
		{
			// 하위 3비트는 READ-ONLY 7번 비트는 존재하지 않음.
			// 그렇다고 날리면 안 됨 =ㅁ=.
			mLCDStatusRegister = ( value & 0b01111000u ) | ( mLCDStatusRegister & 0b10000111u );
			break;
		}
		case 0xff42: // SCY
		{
			mScrollY = value;
			break;
		}
		case 0xff43: // SCX
		{
			mScrollX = value;
			break;
		}
		case 0xff45: // LYC
		{
			mLYC = value;
			break;
		}
		case 0xff46: // DMA
		{
			mDMASourceHi = value; // High Values;
			mIsDMAStart = true;
			break;
		}
		case 0xff47:
		{
			mBGMonoPallet = value;
			break;
		}
		case 0xff48:
		{
			mOBJMonoPallet0 = value;
			break;
		}
		case 0xff49:
		{
			mOBJMonoPallet1 = value;
			break;
		}
		case 0xff4a: // WY
		{
			mWY = value;
			break;
		}
		case 0xff4b: // WX
		{
			mWX = value;
			break;
		}
		case 0xff4f:
		{
			mSelectVRAMBank = ( value & 0x01u );
			break;
		}
		case 0xff51:
		{
			mHDMASourceHi = value;
			break;
		}
		case 0xff52:
		{
			mHDMASourceLo = value;
			break;
		}
		case 0xff53:
		{
			mHDMADestHi = value;
			break;
		}
		case 0xff54:
		{
			mHDMADestLo = value;
			break;
		}
		case 0xff55:
		{
			mHDMAStatus = value;
			mIsHDMAStart = true;
			break;
		}
		case 0xff68: // BG Pallet Index Select
		{
			mBGColorPalletIndex = value;
			break;
		}
		case 0xff69: // BG Pallet
		{
			// 팔렛트 BYTE []
			// 0 -> ( gggrrrrr )
			// 1- > ( 0bbbbbgg )
			// Bit 0-4   Red Intensity   (00-1F)
			// Bit 5-9   Green Intensity (00-1F)
			// Bit 10-14 Blue Intensity  (00-1F)
			// 이건 쓸때랑 받을떄 알아서 해석할 것 = ㅁ=
			BYTE only_pallet_index = toOnlyPalletIndex( mBGColorPalletIndex ); // 실제로는 3f만 쓸 수 있음.
			BYTE to_color_index = toColorIndex( only_pallet_index );
			BYTE to_pallet = toPalletIndex( only_pallet_index, to_color_index );

			bool isLo = only_pallet_index % 2 == 0;

			if ( isLo )
			{
				mBGColorPallet[ to_pallet ][ to_color_index ].SetLo( value );
			}
			else
			{
				mBGColorPallet[ to_pallet ][ to_color_index ].SetHi( value );
			}

			autoIncrementPalletIndex( mBGColorPalletIndex );
			break;
		}
		case 0xff6a: // Object Pallet Index Select
		{
			mObjectColorPalletIndex = value;
			break;
		}
		case 0xff6b:
		{
			BYTE only_pallet_index = toOnlyPalletIndex(mObjectColorPalletIndex); // 실제로는 3f만 쓸 수 있음.
			BYTE to_color_index = toColorIndex( only_pallet_index );
			BYTE to_pallet = toPalletIndex( only_pallet_index, to_color_index );

			bool isLo = only_pallet_index % 2 == 0;

			if ( isLo )
			{
				mObjectColorPallet[ to_pallet ][ to_color_index ].SetLo( value );
			}
			else
			{
				mObjectColorPallet[ to_pallet ][ to_color_index ].SetHi( value );
			}

			autoIncrementPalletIndex( mObjectColorPalletIndex );
			break;
		}
		default:
			throw std::logic_error("Not Impl Interrupt : " + std::to_string( mem_addr ) );
			break;
	}
}


BYTE GPU::procInterruptsOnGet(size_t mem_addr) const
{
	switch ( mem_addr )
	{
		case 0xff40: // LCD Control Register
		{
			return mLCDControlRegister;
		}
		case 0xff41: // LCD Status
		{
			return mLCDStatusRegister;
		}
		case 0xff42: // SCY
		{
			return mScrollY;
		}
		case 0xff43: // SCX
		{
			return mScrollX;
		}
		case 0xff44: // LY
		{
			return mScanLineY;
		}
		case 0xff45: // LYC
		{
			return mLYC;
		}
		case 0xff46: // DMA Get, -> DO NOTHING.
		{
			return 0;
		}
		case 0xff47:
		{
			return mBGMonoPallet;
		}
		case 0xff48:
		{
			return mOBJMonoPallet0;
		}
		case 0xff49:
		{
			return mOBJMonoPallet1;
		}
		case 0xff4a: // WY
		{
			return mWY;
		}
		case 0xff4b: // WX
		{
			return mWX;
		}
		case 0xff4f: // RAM BANK
		{
			return ( 0xfeu ) | ( mSelectVRAMBank );
		}
		case 0xff51: // DMA Source Hi
		{
			return mHDMASourceHi;
		}
		case 0xff52: // DMA Source Lo
		{
			return mHDMASourceLo;
		}
		case 0xff53: // DMA Dest Hi
		{
			return mHDMADestHi;
		}
		case 0xff54: // DMA Dest Lo
		{
			return mHDMADestLo;
		}
		case 0xff55: // DMA 끝났는지 알 수 있는 7번 비트
		{
			if ( !mIsHDMAStart )
			{
				return 0x80;
			}
			else
			{
				return 0;
			}
		}
		case 0xff68:
		{
			return mBGColorPalletIndex;
		}
		case 0xff69:
		{
			BYTE only_pallet_index = toOnlyPalletIndex( mBGColorPalletIndex ); // 실제로는 3f만 쓸 수 있음.
			BYTE to_color_index = toColorIndex( only_pallet_index );
			BYTE to_pallet = toPalletIndex( only_pallet_index, to_color_index );

			bool isLo = only_pallet_index % 2 == 0;

			if (isLo)
			{
				return mBGColorPallet[ to_pallet ][ to_color_index ].GetLo();
			}
			else
			{
				return mBGColorPallet[ to_pallet ][ to_color_index ].GetHi();
			}

		}
		case 0xff6a:
		{
			return mObjectColorPalletIndex;
		}
		case 0xff6b:
		{
			BYTE only_pallet_index = toOnlyPalletIndex(mObjectColorPalletIndex); // 실제로는 3f만 쓸 수 있음.
			BYTE to_color_index = toColorIndex( only_pallet_index );
			BYTE to_pallet = toPalletIndex( only_pallet_index, to_color_index );

			bool isLo = only_pallet_index % 2 == 0;

			if (isLo)
			{
				return mObjectColorPallet[ to_pallet ][ to_color_index ].GetLo();
			}
			else
			{
				return mObjectColorPallet[ to_pallet ][ to_color_index ].GetHi();
			}
		}
		default:
			throw std::logic_error("Not Impl Interrupt, Get : " + std::to_string( mem_addr ) );
	}

	return 0;
}


void GPU::checkAddress(size_t mem_addr) const
{
	int result_relative_address = static_cast<int>( mem_addr ) - static_cast<int>( VRAM_START_ADDRESS );

	if (result_relative_address < 0 )
	{
		throw std::logic_error("UNDERFLOW, ADDRESS");
	}
	if (result_relative_address >= mMemory[mSelectVRAMBank].size())
	{
		throw std::logic_error("OVERFLOW, ADDRESS.");
	}
}

void GPU::enableVBlank()
{
	SetBit(mLCDStatusRegister, 4 );
}

void GPU::disableVBlank()
{
	OffBit(mLCDStatusRegister, 4 );
}

void GPU::enableHBlank()
{
	SetBit( mLCDStatusRegister, 3 );
}

void GPU::disableHBlank()
{
	OffBit( mLCDStatusRegister, 3 );
}

void GPU::setCoincidenceInterrupt(bool value)
{
	if ( value )
	{
		SetBit( mLCDStatusRegister, 2 );
	}
	else
	{
		OffBit( mLCDStatusRegister, 2 );
	}
}

void GPU::setLCDMode(BYTE mode)
{
	mLCDStatusRegister = ( mLCDStatusRegister & 0xfc ) | mode;
}

void GPU::setRgb(GPUHelper::ColorPallet pallet, BYTE x)
{
	mColorScreen[mScanLineY][x] = pallet;
}

// 8의 배수로 변경함.
// 9 -> 8
// 15 -> 8
// 16 -> 8
inline BYTE CovertToMultipleOf8( BYTE value )
{
	return ( value - ( value % 8 ) ) / 8;
}

void GPU::drawBackground()
{
	// -7 뺴는 것이 국룰.
	BYTE window_x = mWX - 7;
	BYTE window_y = mWY;

	// W 좌표 계산
	BYTE pixel_y = mScrollY + mScanLineY;

	// 0~31로 사상됨
	BYTE tile_y = ( pixel_y - ( pixel_y % 8 ) ) / 8;

	bool window_enable = IsWindowDisplayEnable() && mScanLineY >= window_y; // 현재 스캔 라인이 윈도우보다 크면

	for ( int i = 0; i < GPUHelper::ScreenWidth; i++ )
	{
		WORD base_tile_map  = 0x9800u;

		if( ( GetSelectBGTileMapDisplay() == 0x9C00u && ( i > window_x || !window_enable ) ) || // BG가 9C00u가 선택된 상태로, 아직 윈도우 그릴 차례가 아님.
				( GetSelectedWindowTileMap() == 0x9C00u && i <= window_x && window_enable ) ) // WindowTileMap이 9C00 선택되었고. 윈도우 그릴 차례임.
		{
			base_tile_map = 0x9C00u;
		}


		BYTE pixel_x = mScrollX + i;
		// 256x256 ->  32 x 32
		// 8을 버리고, 8을 나눠서 0~32로.
		BYTE tile_x = CovertToMultipleOf8(pixel_x);

		/* 2바이트 == 8 비트만큼 Width, BG는 아래와 같이 되어있음.
		 * [0]  ( 2바이트 ) * 16 [31]
		 * [32] ( 2바이트 ) * 16 [63]
		 * ....
		 */

		BYTE tile_index = Get(base_tile_map + tile_x + (tile_y * 32));
		WORD base_tile_addr = GetSelectedTileAddress(tile_index );
		WORD tile_addr = (base_tile_addr + ( (pixel_y % 8 ) * 2)); // 8x8중 몇번째 라인인가? (pixel_y%8) 그런데 2바이트 단위니까 2를 곱해줌.

		std::array<BYTE, 8> pallets = GPUHelper::ToTileData(Get(tile_addr),Get(tile_addr + 1) );

		// 일단 모노만 짜놓고 생각하자

		if (true) // TODO : 여기 모노 구분자 추가
		{
			GPUHelper::MonoPallet pallet_result = GPUHelper::GetPalletData( mBGMonoPallet, pallets[pixel_x % 8] );
			mMonoScreen[mScanLineY][i] = pallet_result;
		}
	}
}

void GPU::drawSprites()
{
	constexpr BYTE END_RANGE = 7;

	std::vector<GPUHelper::ObjectAttribute> object_attributes;

	for( int i = 0; i < 20; i++ ) // 이번 라인에 그릴 애들을 찾음.
	{
		GPUHelper::ObjectAttribute attr = GetObjectAttribute( i );
		if( attr.y_position <= ( mScanLineY )  && ( attr.y_position  + END_RANGE ) >= ( mScanLineY )  )
		{
			object_attributes.emplace_back( std::move(attr) );
		}
	}

	if( object_attributes.empty() ) { return; } // 이번 라인에는 그릴 게 없음

	for( int i = 0; i < GPUHelper::ScreenWidth; i++ )
	{
		// Y는 위에서 계산했음.
		for ( const GPUHelper::ObjectAttribute & ref_attribute : object_attributes )
		{
			// 이번에 그려야 함
			if (ref_attribute.x_position <= i && ref_attribute.x_position + END_RANGE >= i )
			{
				// 이 값들은 0~7범위 일 수 밖에 없다.
				BYTE x_index = i - ref_attribute.x_position; // 몇번째로 그리고 있는가?
				BYTE y_index = mScanLineY - ref_attribute.y_position; // Y축은 몇번째인가?

				BYTE tile = ref_attribute.sprite_tile_number;
				WORD tile_start_address = GPUHelper::SpriteTileStartAddress + (tile * GPUHelper::TileDataLSize); // 16바이트 * 0x0~0xff = 0x1000. 타일 주소와 일치함.
				WORD tile_address = ( y_index * 2 ) + tile_start_address;

				//일단 모노만..
				std::array<BYTE, 8> pallets = GPUHelper::ToTileData(Get(tile_address),
														Get(tile_address + 1) );
				if ( true )
				{
					if ( pallets[x_index] != 0 ) // 0이면 스킵
					{
						BYTE pallet_number = ref_attribute.attributes.cgb_pallet_number;
						if (pallet_number == 0 )
						{
							GPUHelper::MonoPallet pallet_result = GPUHelper::GetPalletData( mOBJMonoPallet0, pallets[x_index] );
							mMonoScreen[mScanLineY][i] = pallet_result;
						}
						else
						{
							GPUHelper::MonoPallet pallet_result = GPUHelper::GetPalletData( mOBJMonoPallet1, pallets[x_index] );
							mMonoScreen[mScanLineY][i] = pallet_result;
						}
					}
				}
			}
		}
	}

}


void GPU::autoIncrementPalletIndex(BYTE &pallet_index)
{
	bool auto_increment = ( pallet_index & 0b10000000u ) == 0b10000000u;

	if ( auto_increment )
	{
		pallet_index++;
	}
}

BYTE GPU::toOnlyPalletIndex(BYTE pallet_index)
{
	return pallet_index & 0x3fu;
}

BYTE GPU::toColorIndex(BYTE only_pallet_index)
{
	return only_pallet_index % 4;
}

BYTE GPU::toPalletIndex(BYTE only_pallet_index, BYTE color_index)
{
	return ( only_pallet_index - ( color_index ) ) / 4;
}
