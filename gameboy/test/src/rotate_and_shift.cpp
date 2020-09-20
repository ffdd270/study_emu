#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"


TEST_CASE( "ROTATE AND SHIFT", "[ROTATE AND SHIFT]" )
{
	GameboyCPU cpu;

	SECTION("RLC m") // Rotate Left Though Carry.
	{
		SECTION( "RLC A" )
		{
			rlcRegister( cpu, 0b10001000, 0b111 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetRegisterAF().hi == 0b00010001 );
		}

		SECTION( "RLC (HL)")
		{
			rlcMemoryHL( cpu, 0b10001000, 0x2401 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetMemoryValue( 0x2401 )  == 0b00010001 );
		}
	}

}