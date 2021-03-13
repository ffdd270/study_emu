#include <catch.hpp>
#include "Motherboard.h"
#include "memory/Timer.h"


SCENARIO("Timer", "[TMR]")
{
	GIVEN("A Single Timer")
	{
		Timer timer;


		WHEN("After Clock 0")
		{
			timer.NextStep( 0 );

			THEN("DIV Register == 0")
			{
				REQUIRE( timer.Get( 0xff04 ) == 0 );
			}
		}

		WHEN("After Clock 256.")
		{
			timer.NextStep( 256 );

			THEN("DIV Register == 1")
			{
				REQUIRE( timer.Get( 0xff04 ) == 1 );
			}
		}

		WHEN("After Clock 256, and Write 0xff04")
		{
			timer.NextStep( 256 );
			timer.Set( 0xff04, 1 ); //어떤 값이던 쓰기만 하면 된다.

			THEN("DIV Register == 0")
			{
				REQUIRE( timer.Get( 0xff04 ) == 0 );
			}
		}
	}
}
