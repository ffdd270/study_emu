#include <catch.hpp>
#include "GameboyCPU.h"

SCENARIO(  "Gameboy Public Methods", "[PUBLIC METHODS]" )
{
	GameboyCPU cpu;

	GIVEN("Memory")
	{
		cpu.InjectionMemory(0x34); // 0x1000 = 0x34
		auto memory = cpu.GetMemory();

		WHEN("Get a Memory")
		{
			 REQUIRE_NOTHROW( memory.GetValue( 0x1000 ) );
			 BYTE memory_value =  memory.GetValue( 0x1000 );

			 THEN("0x34.")
			{
			 	REQUIRE( memory_value == 0x34 );
			}
		}

		WHEN("Out of index")
		{
			REQUIRE_THROWS( memory.GetValue( 0xffff ) );
		}
	}
}