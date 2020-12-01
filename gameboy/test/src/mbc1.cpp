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

				THEN("RAM is Enabled.")
				{
					REQUIRE( mbc1.IsRAMActive() );
				}
			}
		}

		SECTION("Set Rom Bank Least 5Bit.  (0x2000~0x3fff)")
		{
			WHEN("Write to 0x2000, 0b10111111")
			{
				BYTE set_value = 0b10111111u;
				BYTE expect_value = set_value & 0b00011111u; // 5Bit 빼고 제거.

				mbc1.Set( 0x2000, set_value );

				THEN("ROM BANK same as expect value.")
				{
					REQUIRE( mbc1.GetRomBankNumber() == expect_value );
				}
			}

			WHEN( "Write to 0x32f3, 0b0.")
			{
				mbc1.Set( 0x2000, 0 );
				THEN("ROM BANK is 1!")
				{
					REQUIRE(mbc1.GetRomBankNumber() == 1);
				}
			}
		}

		SECTION("Set Rom Bank Hi 2Bit. (0x4000~0x5fff)")
		{
			WHEN("Write to 0x5000, 0x0f.")
			{
				BYTE set_value = 0x0f;
				BYTE expect_value = ( set_value & 0x03u ) << 5u;
				mbc1.Set( 0x5000, set_value );

				THEN("ROM BANK : ( 0x0f & 0x03 ) << 5u")
				{
					// 기본 값 + .
					REQUIRE( mbc1.GetRomBankNumber() == ( expect_value + 1 ) );
				}

			}
		}


	}
}