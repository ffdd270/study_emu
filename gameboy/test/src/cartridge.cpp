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
	}
}