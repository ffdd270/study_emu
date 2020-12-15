#include <catch.hpp>
#include "GameboyCPU.h"
#include "memory/GPU.h"


SCENARIO("GPU", "[GPU]")
{
	GIVEN("A Single GPU")
	{
		GPU vram;

		WHEN("Write 0x1200, 3")
		{
			vram.Set( 0x1200, 3 );

			THEN("Read 0x2000 == 3 ")
			{
				REQUIRE( vram.Get( 0x1200 ) == 3 );
			}
		}

		WHEN("Write 0x2001, 3")
		{
			THEN("THROW.")
			{
				REQUIRE_THROWS( vram.Set( 0x2001, 3 ) );
			}
		}

		WHEN("Read 0x2001")
		{
			THEN("THROW")
			{
				REQUIRE_THROWS( vram.Get(0x2001) );
			}
		}
	}


}