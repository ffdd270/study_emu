#include <catch.hpp>
#include "memory_bank_controllers/MBC1.h"

SCENARIO("Use MBC1.", "[MBC]")
{
	GIVEN("A Single cpu_instrs.gb. Use MBC1.")
	{
		Cartridge cart;
		REQUIRE_NOTHROW( cart.Load( "roms/cpu_instrs.gb" ) );
		REQUIRE( cart.GetCartridgeType() == 0x01 ); // MBC1!

		MBC1 mbc1( std::move(cart) );
		SECTION( "Set Ram Enabled." )
		{
			WHEN("write to 0x02ff, 0xa.")
			{
				mbc1.Set( 0x02ff, 0xA );

				THEN("RAM is Enabled!")
				{
					REQUIRE( mbc1.IsRAMActive() );
				}
			}

			WHEN("write to 0x03ff, 0x0.")
			{
				mbc1.Set( 0x03ff, 0x0 );

				THEN("RAM is disabled.")
				{
					REQUIRE_FALSE( mbc1.IsRAMActive() );
				}
			}

			WHEN("write to 0x14ff, 0x0.")
			{
				mbc1.Set( 0x02ff, 0xA );

				THEN("RAM is disabled.")
				{
					REQUIRE( mbc1.IsRAMActive() );
				}
			}
		}
	}
}