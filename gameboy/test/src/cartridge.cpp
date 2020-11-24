#include <catch.hpp>
#include "Cartridge.h"

SCENARIO( "Cartridge Test", "[CART]" )
{
	Cartridge cart;

	GIVEN( "A cpu_instrs.gb" )
	{
		REQUIRE_NOTHROW( cart.Load( "roms/cpu_instrs.gb" ) );
	}
}