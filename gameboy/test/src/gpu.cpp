#include <catch.hpp>
#include "GameboyCPU.h"
#include "memory/GPU.h"

inline void pallet_test( GPU & gpu, WORD pallet_index_address, WORD pallet_address, BYTE red, BYTE green, BYTE blue )
{
	// PalletIndex = 0x2, Auto increment. = 0x80 ( BIT 7 )
	gpu.Set( pallet_index_address, 0x82 );

	// PalletIndex == 2?
	REQUIRE( ( gpu.Get( pallet_index_address ) & 0x1fu ) == 0x2 );

	// Green Lo 3 Bit | red 5 bit.
	gpu.Set( pallet_address, ( green & 0x07u ) << 5u | red );
	// Auto increment.
	REQUIRE( ( gpu.Get( pallet_index_address ) & 0x1fu ) == 0x3 );

	// blue 5bit. | Green hi 2 bit
	gpu.Set( pallet_address, static_cast<BYTE>( blue << 0x2u ) | static_cast<BYTE>( static_cast<BYTE>( green & 0x18u ) >> 3u ) );
	REQUIRE( ( gpu.Get( pallet_index_address ) & 0x1fu ) == 0x4 );

	THEN("Get Pallet, Then R = 0x1f, G = 0x0f, B = 0x10")
	{
		gpu.Set( pallet_index_address, 0x82 ); // Pallet Index = 0x2, Auto Increment
		BYTE lo = gpu.Get( pallet_address );
		REQUIRE(( gpu.Get( pallet_index_address ) & 0x1fu ) == 0x02 ); // Get methods no auto increment.

		gpu.Set( pallet_index_address, 0x03 ); // Pallet Index = 0x3.
		BYTE hi = gpu.Get( pallet_address );
		REQUIRE( gpu.Get( pallet_index_address ) == 0x03 );

		GPUHelper::ColorPallet pallet;
		pallet.SetLo( lo );
		pallet.SetHi( hi );

		REQUIRE( pallet.Red() == red );
		REQUIRE( pallet.Green() == green );
		REQUIRE( pallet.Blue() == blue );
	}
}


SCENARIO("GPU", "[GPU]")
{
	GIVEN("A Single GPU, Test Memory Access.")
	{
		GPU gpu;

		WHEN("Write 0x8200, 3")
		{
			gpu.Set(0x8200, 3 );

			THEN("Read 0x8200 == 3 ")
			{
				REQUIRE(gpu.Get( 0x8200 ) == 3 );
			}
		}

		WHEN("Write 0xA001, 3")
		{
			THEN("THROW.")
			{
				REQUIRE_THROWS(gpu.Set(0xA001, 3 ) );
			}
		}

		WHEN("Read 0xA001")
		{
			THEN("THROW")
			{
				REQUIRE_THROWS(gpu.Get(0xA001) );
			}
		}

		WHEN("Write 0xff40, 0b11011011")
		{
			gpu.Set( 0xff40, 0b11010011u );

			THEN("7, 6, 4, 1, 0 SET, Else -> NOT SET")
			{
				BYTE LCDC = gpu.Get( 0xff40 );

				REQUIRE(GPUHelper::IsLCDDisplayEnable(LCDC ) );
				REQUIRE(GPUHelper::GetSelectedWindowTileMap(LCDC ) == 0x9C00 );
				REQUIRE_FALSE(GPUHelper::IsWindowDisplayEnable(LCDC ) );
				REQUIRE(GPUHelper::GetSelectBGAndWindowTileData(LCDC ) == 0x8000 );
				REQUIRE(GPUHelper::GetSelectBGTileMapDisplay(LCDC ) == 0x9800 );
				REQUIRE_FALSE(GPUHelper::IsSpriteSize(LCDC ) );
				REQUIRE(GPUHelper::IsSpriteDisplayEnable(LCDC ) );
				REQUIRE(GPUHelper::CheckProperty(LCDC ) );
			}
		}

		WHEN("Write 0xff41, ( LCD Status Register ) , 0b01101111")
		{
			gpu.Set( 0xff41, 0b01101111u );

			THEN("6,5,3 Seteed, 2, 1, 0 -> READ ONLY, not chaged." )
			{
				BYTE LCDStatusRegister = gpu.Get( 0xff41 );

				REQUIRE(GPUHelper::IsEnableLYCoincidenceInterrupt(LCDStatusRegister ) );
				REQUIRE(GPUHelper::IsEnableMode2OAMInterrupt(LCDStatusRegister ) );
				REQUIRE(GPUHelper::IsEnableMode1VBlankInterrupt(LCDStatusRegister ) == false );
				REQUIRE(GPUHelper::IsEnableMode0HBlankInterrupt(LCDStatusRegister ) );
				REQUIRE(GPUHelper::IsCoincidence(LCDStatusRegister ) == false );
				REQUIRE(GPUHelper::GetModeFlag(LCDStatusRegister ) == 0 );
			}
		}

		WHEN("Write 0xff42, 0xff43. ( SCY, SCX ) 20, 30")
		{
			gpu.Set( 0xff42, 20 );
			gpu.Set( 0xff43, 30 );

			THEN("Get 0xff42, 0xff43 , 20, 30.")
			{
				REQUIRE( gpu.Get( 0xff42 ) == 20 );
				REQUIRE( gpu.Get( 0xff43 ) == 30 );
			}
		}

		WHEN("Write 0xff4a, 0xff4b, ( WY, WX ), 45, 51 ")
		{
			gpu.Set( 0xff4a, 45 );
			gpu.Set( 0xff4b, 51 );

			THEN("Get 0xff4a, 0xff4b, 45, 51")
			{
				REQUIRE( gpu.Get( 0xff4a ) == 45 );
				REQUIRE( gpu.Get( 0xff4b ) == 51 );
			}
		}

		WHEN ("Write 0xff47`0xff49 (Mono Pallet)")
		{
			gpu.Set( 0xff47, 0b11100100 ); // BLACK, DARK GRAY, LIGHT GRAY, WHITE.
			gpu.Set( 0xff48, 0xff ); // ALL BLACK.
			gpu.Set( 0xff49, 0x0 ); // ALL WHITE.

			THEN("Check Color Result : Ok")
			{
				BYTE bg = gpu.Get( 0xff47 );

				REQUIRE( GPUHelper::GetPalletData( bg, 0 ) == GPUHelper::MonoPallet::WHITE );
				REQUIRE( GPUHelper::GetPalletData( bg, 1 ) == GPUHelper::MonoPallet::LIGHT_GRAY );
				REQUIRE( GPUHelper::GetPalletData( bg, 2 ) == GPUHelper::MonoPallet::DARK_GRAY );
				REQUIRE( GPUHelper::GetPalletData( bg, 3 ) == GPUHelper::MonoPallet::BLACK );

				BYTE obj0 = gpu.Get( 0xff48 );
				for( int i = 0; i < 4; i++ )
				{
					REQUIRE( GPUHelper::GetPalletData( obj0, i ) == GPUHelper::MonoPallet::BLACK );
				}

				BYTE obj1 = gpu.Get( 0xff49 );
				for( int i = 0; i < 4; i++ )
				{
					REQUIRE( GPUHelper::GetPalletData( obj1, i ) == GPUHelper::MonoPallet::WHITE );
				}
			}
		}

		WHEN("Set BG Pallet R = 0x1f, G = 0x0f, B = 0x10")
		{
			pallet_test( gpu, 0xff68u, 0xff69u, 0x1f, 0x0f, 0x10 );
		}

		WHEN("Set Obj Pallet R = 0x02, G = 0x07, B = 0x04")
		{
			pallet_test( gpu, 0xff6au, 0xff6bu, 0x02, 0x07, 0x04 );
		}
	}

	GIVEN("A Single GPU, Test NextStep")
	{
		GPU gpu;

		REQUIRE( gpu.GetModeFlag() == 0 );
		REQUIRE( gpu.IsEnableMode1VBlankInterrupt() == false );

		constexpr size_t ONE_LINE_DRAW_CLOCK = 456;

		WHEN("Clock = 114 * 144 After.")
		{
			constexpr size_t MAX_SCANLINE = 144;

			gpu.NextStep( ONE_LINE_DRAW_CLOCK * MAX_SCANLINE );

			THEN(", VBLANK is Active.")
			{
				REQUIRE( gpu.GetModeFlag() == 1 );
				REQUIRE( gpu.IsEnableMode1VBlankInterrupt() );
			}

			THEN("LY = 144")
			{
				REQUIRE( gpu.Get( 0xff44 ) == 144 );
			}
		}

		WHEN("LYC = 100, Step 114 * 100, -> LYC == LY Interrupt.")
		{
			gpu.Set( 0xff41, 0b1u << 6u ); // Set Enable LY Coincidence Interrupt BIT 6.
			REQUIRE( gpu.IsEnableLYCoincidenceInterrupt() );

			constexpr size_t LYC = 100;
			gpu.Set( 0xff45, LYC );

			REQUIRE( gpu.Get( 0xff45 ) == LYC );
			REQUIRE( gpu.IsCoincidence() == false );

			gpu.NextStep( ONE_LINE_DRAW_CLOCK * LYC  );

			THEN(", LY Interrupt On.")
			{
				REQUIRE( gpu.IsCoincidence() );
			}
		}
	}
}