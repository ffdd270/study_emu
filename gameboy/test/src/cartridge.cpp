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
	}
}