#include <catch.hpp>
#include "GameboyCPU.h"
#include "memory/GPU.h"


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