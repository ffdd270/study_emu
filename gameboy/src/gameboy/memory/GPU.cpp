//
// Created by HaruGakkaP on 2020-12-15.
//

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
	origin & ( 0xFFu ^ ( 0b1u << bit_pos ) );
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

GPU::GPU() : mMemory( { 0 } ), mLCDStatusRegister( 0 ), mLCDControlRegister( 0 ), mDots( 0 ), mScanLineY( 0 ), mScrollX( 0 ), mScrollY( 0 ), mLYC( 0 )
{

}

constexpr size_t VRAM_START_ADDRESS = 0x8000;
// 0x8000~0x9fff
BYTE GPU::Get(size_t mem_addr) const
{
	if( mem_addr == 0xff40 ) // LCD Control Register
	{
		return mLCDControlRegister;
	}
	else if( mem_addr == 0xff41 ) // LCD Status
	{
		return mLCDStatusRegister;
	}
	else if( mem_addr == 0xff42 ) // SCY
	{
		return mScrollY;
	}
	else if( mem_addr == 0xff43 ) // SCX
	{
		return mScrollX;
	}
	else if( mem_addr == 0xff44 ) // LY
	{
		return mScanLineY;
	}
	else if( mem_addr == 0xff45 ) // LYC
	{
		return mLYC;
	}
	else if (mem_addr == 0xff47 )
	{
		return mBGMonoPallet;
	}
	else if (mem_addr == 0xff48 )
	{
		return mOBJMonoPallet0;
	}
	else if ( mem_addr == 0xff49 )
	{
		return mOBJMonoPallet1;
	}
	else if( mem_addr == 0xff4a ) // WY
	{
		return mWY;
	}
	else if( mem_addr == 0xff4b ) // WX
	{
		return mWX;
	}
	else // VRAM
	{
		checkAddress(mem_addr);
		return mMemory[mem_addr - VRAM_START_ADDRESS];
	}
}


void GPU::Set(size_t mem_addr, BYTE value)
{
	if( mem_addr == 0xff40 ) // LCD Control Register
	{
		mLCDControlRegister = value;
	}
	else if( mem_addr == 0xff41 ) // LCD Status
	{
		// 하위 3비트는 READ-ONLY 7번 비트는 존재하지 않음.
		// 그렇다고 날리면 안 됨 =ㅁ=.
		mLCDStatusRegister = ( value & 0b01111000u ) | ( mLCDStatusRegister & 0b10000111u );
	}
	else if( mem_addr == 0xff42 ) // SCY
	{
		mScrollY = value;
	}
	else if( mem_addr == 0xff43 ) // SCX
	{
		mScrollX = value;
	}
	else if( mem_addr == 0xff45 ) // LYC
	{
		mLYC = value;
	}
	else if (mem_addr == 0xff47 )
	{
		mBGMonoPallet = value;
	}
	else if (mem_addr == 0xff48 )
	{
		mOBJMonoPallet0 = value;
	}
	else if ( mem_addr == 0xff49 )
	{
		mOBJMonoPallet1 = value;
	}
	else if( mem_addr == 0xff4a ) // WY
	{
		mWY = value;
	}
	else if( mem_addr == 0xff4b ) // WX
	{
		mWX = value;
	}
	else // VRAM
	{
		checkAddress(mem_addr);
		mMemory[mem_addr - VRAM_START_ADDRESS] = value;
	}
}

void GPU::NextStep(size_t clock)
{
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

	int loop_target = (static_cast<int>( clock - 1 ) / 80 ) + 1; // 왜 80인가? 라고 물으면..
	// 현대 컴퓨터에서 이걸 일일히 다 따라하기보단, 그냥 모드 전환만 적당히 되면 되서 그런 거 아닐까?
	// 그럼 더 큰값은 안 되냐고 생각할 지도 모르겠지만. 모드 2의 유지 기간이 80이라서. 80으로 해야 모든 모드들에 진입할 수 있다.

	constexpr size_t LINE_PER_DOTS = 456;
	constexpr size_t REAL_SCANLINE_END = 144;
	constexpr size_t MAX_SCANLINE = 154;

	for( int i = 0; i < loop_target; i++ )
	{
		if ( i == ( clock - 1 ) ) // 마지막이면 80의 나머지 숫자를.
		{
			mDots += clock % 80;
		}
		else // 아니라면 그냥 몫만큼 더하면 됨.
		{
			mDots += 80;
		}

		size_t prv_bots = mDots;
		mDots = mDots % LINE_PER_DOTS;

		if ( prv_bots != mDots ) // 이게 다르다는 게 무슨 뜻이냐면, 라인이 넘어갔다는 뜻이다.
		{
			mScanLineY = ( mScanLineY + 1 ) % MAX_SCANLINE; // 스캔라인은 154까지 있다.

			if ( IsEnableLYCoincidenceInterrupt() ) // 플래그 올라가면 체크.
			{
				setCoincidenceInterrupt(mScanLineY == mLYC); // LYC랑 같으면 인터럽트 발생.
			}
		}

		if ( mScanLineY >= REAL_SCANLINE_END ) //  V-BLANK 이벤트. 이미 스캔라인은 다 그렸지만, CPU들이 VRAM에 접근할 수 있도록
		{
			if( GetModeFlag() == 1 )
			{
				continue; // 계속.
			}

			setLCDMode(1);
			enableVBlank();
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

void GPU::checkAddress(size_t mem_addr) const
{
	int result_relative_address = static_cast<int>( mem_addr ) - static_cast<int>( VRAM_START_ADDRESS );

	if (result_relative_address < 0 ) { throw  std::logic_error("UNDERFLOW, ADDRESS"); }
	if (result_relative_address >= mMemory.size()) { throw std::logic_error("OVERFLOW, ADDRESS."); }
}

void GPU::enableVBlank()
{
	SetBit(mLCDStatusRegister, 4 );
}

void GPU::disableVBlank()
{
	OffBit(mLCDStatusRegister, 4 );
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
	mLCDStatusRegister |= mode;
}