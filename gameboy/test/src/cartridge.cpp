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
	}
}