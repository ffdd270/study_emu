#include <catch.hpp>
#include "Cartridge.h"

SCENARIO( "Cartridge Test", "[CART]" )
{
	Cartridge cart;

	GIVEN( "A cpu_instrs.gb" )
	{
		REQUIRE_NOTHROW( cart.Load( "roms/cpu_instrs.gb" ) );

		WHEN("Get Title")
		{
			std::string name;
			REQUIRE_NOTHROW( name = cart.GetTitle() );

			THEN("Name : CPU_INSTRS")
			{
				REQUIRE( name == "CPU_INSTRS" );
			}
		}

		WHEN("Get CGB Flag")
		{
			Cartridge::ColorGameBoyFlag flag;
			REQUIRE_NOTHROW( flag = cart.GetCGBFlag()  );

			THEN("FLAG : NONE")
			{
				REQUIRE( flag == Cartridge::ColorGameBoyFlag::CGB_SUPPORT );
			}
		}

		WHEN("Get Cartridge Type")
		{
			BYTE type;
			REQUIRE_NOTHROW( type = cart.GetCartridgeType() );

			THEN("TYPE : MBC1")
			{
				REQUIRE( type == 0x01 );
			}
		}

		WHEN("Get Cartridge Size")
		{
			CartridgeSizeInfo info;
			REQUIRE_NOTHROW(info = cart.GetSizeInfo());

			THEN("64kb")
			{
				REQUIRE(info.bank == 4);
				REQUIRE(info.size == 0x10000);
				REQUIRE(cart.GetRealBufferSize() == info.size);
			}
		}

		WHEN( "Get EntryPoint" )
		{
			std::array<BYTE, 4> array = { 0 };
			REQUIRE_NOTHROW( array = cart.GetEntryPoint()  );

			THEN("0x00, 0xc3, 0x37, 0x06")
			{
				REQUIRE( array[0] == 0x00 );
				REQUIRE( array[1] == 0xc3 );
				REQUIRE( array[2] == 0x37 );
				REQUIRE( array[3] == 0x06 );
			}
		}

		WHEN("Access to right address.")
		{
			BYTE data = 0;
			REQUIRE_NOTHROW( data = cart.GetData( 0x3f3 ) );

			THEN("data is 2f")
			{
				REQUIRE( data == 0x2f );
			}
		}

		WHEN("Access to wrong address.")
		{
			size_t out_of_range = cart.GetSizeInfo().size;
			THEN("THROW!")
			{
				REQUIRE_THROWS( cart.GetData( out_of_range ) );
			}
		}

		WHEN("Get Ram Size info.")
		{
			CartridgeSizeInfo info;
			REQUIRE_NOTHROW( info = cart.GetRamSizeInfo() );

			THEN("Not have RAM.")
			{
				REQUIRE(info.bank == 1);
				REQUIRE(info.size == 0);
			}
		}
	}

	GIVEN("RAM 64kb  Cartridge.")
	{
		cart.Load("roms/ram_64kb.gb");

		WHEN("Get Ram Size Info.")
		{
			CartridgeSizeInfo info;
			REQUIRE_NOTHROW(info = cart.GetRamSizeInfo());

			THEN("RAM is 64Kbit.")
			{
				REQUIRE(info.bank == 1);
				REQUIRE(info.size == 8192); // 0x2000
			}
		}

		WHEN("Access to Right RAM Address")
		{
			THEN("No Throw.")
			{
				REQUIRE_NOTHROW( cart.GetRamData( 0x30 ) );
			}
		}

		WHEN( "Access to Wrong RAM Address" )
		{
			THEN("Throw")
			{
				REQUIRE_THROWS( cart.GetRamData( 0x3000 ) );
			}
		}
	}
}