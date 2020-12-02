#include <catch.hpp>
#include "memory_bank_controllers/MBC1.h"

void MBC1GetterTest( MBC1 & mbc1 )
{
	WHEN("Read CARTRIDGE TYPE In bank 0.")
	{
		BYTE result = 0;

		REQUIRE_NOTHROW( result = mbc1.Get( 0x147 ) );

		THEN("Result : MBC1.")
		{
			REQUIRE( result == 0x1 );
		}
	}

	// 아래 테스트들은 ROM 뱅크여야 한다.
	REQUIRE( mbc1.GetBankMode() == BankMode::ROM );

	// 기본 뱅크 선택은 1이라서.
	WHEN("Read Cartridge Type In bank 01, Get 0x4000(0x4000).")
	{
		REQUIRE( mbc1.GetRomBankNumber() == 1 );
		BYTE result = 0;
		REQUIRE_NOTHROW(  result = mbc1.Get( 0x4000 ) );

		THEN(" Result : MBC1")
		{
			REQUIRE( result == 0xC3 );
		}
	}

	WHEN("Read Cartridge Type IN bank 03, Get 0x4500( 0x4000 * 3 + 0x500 )")
	{
		mbc1.Set( 0x2000, 0x03 );
		REQUIRE( mbc1.GetRomBankNumber() == 3 );

		BYTE result = 0;
		REQUIRE_NOTHROW( result = mbc1.Get( 0x4500 ) );

		THEN("BANK 3, 0x500(0xc500) == 0xc0.")
		{
			REQUIRE( result == 0xC0 );
		}
	}
}

void MBC1SetterTest( MBC1 & mbc1 )
{

	SECTION( "Set Ram Enabled. (0x0000~0x1fff)" )
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

	SECTION("Set BankMode. (0x6000~0x7fff)")
	{
		REQUIRE( mbc1.GetBankMode() == BankMode::ROM ); // 기본 값은 ROM.

		WHEN("Write to 0x7000, 0x1")
		{
			mbc1.Set( 0x7000, 0x1 );
			THEN("BANK MODE : RAM")
			{
				REQUIRE( mbc1.GetBankMode() == BankMode::RAM );
			}
		}

		WHEN("Write to 0x7100, 0x0")
		{
			mbc1.Set( 0x7100, 0x0 );
			THEN("BANK MODE : ROM")
			{
				REQUIRE( mbc1.GetBankMode() == BankMode::ROM );
			}
		}

		WHEN("Write to 0x6000, 0x2 ")
		{
			THEN("THROW!")
			{
				REQUIRE_THROWS( mbc1.Set(0x6000, 0x2) );
			}
		}
	}

	WHEN("Low => 0b11010101 ( SET 10101 ), Hi => 0b11111111 ( SET 0b11 ) ")
	{
		mbc1.Set(0x22ff, 0b11010101);
		mbc1.Set(0x5500, 0xff);

		THEN(" Result => 0b1110101")
		{
			REQUIRE(mbc1.GetRomBankNumber() == 0b1110101);
		}
	}

}


SCENARIO("Use MBC1.", "[MBC]")
{
	GIVEN("A Single cpu_instrs.gb. Use MBC1.")
	{
		Cartridge cart;
		REQUIRE_NOTHROW( cart.Load( "roms/cpu_instrs.gb" ) );
		REQUIRE( cart.GetCartridgeType() == 0x01 ); // MBC1!

		MBC1 mbc1( std::move(cart) );

		SECTION("Setter")
		{
			MBC1SetterTest( mbc1 );
		}

		SECTION("Getter")
		{
			MBC1GetterTest( mbc1 );
		}
	}
}