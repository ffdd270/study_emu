#include <catch.hpp>
#include "Motherboard.h"
#include "memory/Timer.h"
#include "memory/MemoryManageUnit.h"
#include "util.h"

SCENARIO("Timer", "[TMR]")
{
	GIVEN("A Single Timer")
	{
		Timer timer;

		WHEN("Just A R/W Test, 0xff06")
		{
			timer.Set( 0xff06, 0xf );

			THEN("Just Remain 3 BIT Value.")
			{
				REQUIRE( timer.Get( 0xff06 ) == 0xf );
			}
		}

		WHEN("Just A R/W Test, 0xff07")
		{
			timer.Set( 0xff07, 0xf );

			THEN("Just Remain 3 BIT Value.")
			{
				REQUIRE( timer.Get( 0xff07 ) == 0x7 );
			}
		}

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

	GIVEN("Timer NOT ENABLE")
	{
		Timer timer;
		timer.Set( 0xff07, 0 );

		WHEN("Next Step, 256 * 10 ")
		{
			timer.NextStep( 2560 );

			THEN("DIV == 10, TIMER == 0")
			{
				REQUIRE( timer.Get(0xff04) == 10 );
				REQUIRE( timer.Get(0xff05) == 0 );
			}
		}
	}

	GIVEN("Timer ENABLE, Clock => Begin 1024, Timer Value == 1")
	{
		Timer timer;
		timer.Set( 0xff07, 0x4 ); // Only Enable.
		timer.NextStep( 1024 );

		THEN("Check Value.")
		{
			REQUIRE( timer.Get( 0xff04 ) == 4 );
			REQUIRE( timer.Get( 0xff05 ) == 1 );
		}

		WHEN("Fix Clock Mode => 1024 to 16")
		{
			timer.Set( 0xff07, 0b101 );

			THEN("But Same Timer Value")
			{
				REQUIRE( timer.Get( 0xff05 ) == 1 );
			}
		}

		WHEN("Fix, and Clock. => 1024 to 16")
		{
			timer.Set( 0xff07, 0b101 );
			timer.NextStep( 16 );

			THEN("INC 1.")
			{
				REQUIRE( timer.Get( 0xff05 ) == 2 );
			}
		}

		WHEN("Clock => 16 => Clock => 64 => Clock => 256 => Clock ")
		{
			BYTE prv_value = timer.Get( 0xff05 );
			timer.NextStep( 1024 );

			timer.Set( 0xff07, 0b101 );
			timer.NextStep( 16 );

			timer.Set( 0xff07, 0b110 );
			timer.NextStep( 64 );

			timer.Set( 0xff07, 0b111 );
			timer.NextStep( 256 );

			THEN("INC 4.")
			{
				REQUIRE( ( prv_value + 4 ) == timer.Get( 0xff05 ) );
			}
		}
	}

	GIVEN("Timer Modulo => 0x3f, Timer Clock => 0xff")
	{
		Timer timer;
		constexpr BYTE TIMER_MODULO = 0x3f;

		timer.Set( 0xff07, 0x4 ); // Only Enable.
		timer.Set( 0xff06, TIMER_MODULO );
		timer.Set( 0xff05, 0xff );

		WHEN("Try Overflow")
		{
			REQUIRE( timer.Get( 0xff05 ) ==  0xff );
			timer.NextStep( 1024 );

			THEN("Overflow Check.")
			{
				REQUIRE( timer.Get( 0xff05 ) == TIMER_MODULO );
			}
		}
	}

	GIVEN("A Single Motherboard")
	{
		Motherboard motherboard;
		std::shared_ptr<MemoryManageUnit> mmunit_ptr = std::static_pointer_cast<MemoryManageUnit>(motherboard.GetInterface(Motherboard::Interface_MMUNIT));
		std::shared_ptr<MemoryInterface> dummy_memory_ptr = std::static_pointer_cast<MemoryInterface>( std::make_shared<MockRWMemory>() );
		motherboard.SetCartridge( dummy_memory_ptr );
		std::shared_ptr<Timer> timer = std::static_pointer_cast<Timer>( motherboard.GetInterface( Motherboard::Interface_TIMER ) );

		// Set Timer Enable
		timer->Set( 0xff07, 0b100 );

		WHEN("Overflowed Timer")
		{
			timer->NextStep( 1024 * 0x100 );
			REQUIRE( timer->IsReportedInterrupt() );
			motherboard.Step(); //인터럽트 전파.

			THEN("0xff0f, -> Interrupt Flag. BIT 2 ON.")
			{
				REQUIRE( mmunit_ptr->Get( 0xff0f ) == 0b100 );
				REQUIRE( timer->IsReportedInterrupt() == false );
			}
		}
	}
}
