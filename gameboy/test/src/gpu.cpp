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
				REQUIRE( gpu.IsLCDDisplayEnable() );
				REQUIRE( gpu.GetSelectedWindowTileMap() == 0x9C00 );
				REQUIRE_FALSE( gpu.IsWindowDisplayEnable() );
				REQUIRE( gpu.GetSelectBGAndWindowTileData() == 0x8000 );
				REQUIRE( gpu.GetSelectBGTileMapDisplay() == 0x9800 );
				REQUIRE_FALSE( gpu.IsSpriteSize() );
				REQUIRE( gpu.IsSpriteDisplayEnable() );
				REQUIRE( gpu.CheckProperty() );
			}
		}

		WHEN("Write 0xff41, ( LCD Status Register ) , 0b01101111")
		{
			gpu.Set( 0xff41, 0b01101111u );

			THEN("6,5,3 Seteed, 2, 1, 0 -> READ ONLY, not chaged." )
			{
				REQUIRE( gpu.IsEnableLYCoincidenceInterrupt() );
				REQUIRE( gpu.IsEnableMode2OAMInterrupt() );
				REQUIRE( gpu.IsEnableMode1VBlankInterrupt() == false );
				REQUIRE( gpu.IsEnableMode0HBlankInterrupt() );
				REQUIRE( gpu.GetCoincidenceFlag() == false );
				REQUIRE( gpu.GetModeFlag() == 0 );
			}
		}
	}

	GIVEN("A Single GPU, Test NextStep")
	{
		GPU gpu;

		REQUIRE( gpu.GetModeFlag() == 0 );
		REQUIRE( gpu.IsEnableMode1VBlankInterrupt() == false );

		WHEN("Clock = 114 * 144 After.")
		{
			constexpr size_t ONE_LINE_DRAW_CLOCK = 456;
			constexpr size_t MAX_SCANLINE = 144;

			gpu.NextStep( ONE_LINE_DRAW_CLOCK * MAX_SCANLINE );

			THEN(", VBLANK is Active.")
			{
				REQUIRE( gpu.GetModeFlag() == 1 );
				REQUIRE( gpu.IsEnableMode1VBlankInterrupt() );
			}
		}
	}
}