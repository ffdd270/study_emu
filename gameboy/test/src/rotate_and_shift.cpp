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


			rlcRegister( cpu, 0b01000100, 0b111 );
			check_flags( cpu, false, false, false, false );
			REQUIRE( cpu.GetRegisterAF().hi == 0b10001000 );
		}

		SECTION( "RLC (HL)")
		{
			rlcMemoryHL( cpu, 0b10001000, 0x2401 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetMemoryValue( 0x2401 )  == 0b00010001 );

			rlcMemoryHL( cpu, 0b01000100, 0x2401 );
			check_flags( cpu, false, false, false, false );
			REQUIRE( cpu.GetMemoryValue( 0x2401 )  == 0b10001000 );
		}
	}

	SECTION( "RRC m" )
	{
		SECTION("RRC A")
		{
			rrcRegister( cpu, 0b10001000, 0b111 );
			check_flags( cpu, false, false, false, false );
			REQUIRE( cpu.GetRegisterAF().hi == 0b01000100 );

			rrcRegister( cpu, 0b00010001, 0b111 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetRegisterAF().hi == 0b10001000 );
		}

		SECTION("RRC (HL)")
		{
			rrcMemoryHL( cpu, 0b10001000, 0x765 );
			check_flags( cpu, false, false, false, false );
			REQUIRE( cpu.GetMemoryValue( 0x765 )  == 0b01000100 );

			rrcMemoryHL( cpu, 0b00010001, 0x765 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetMemoryValue( 0x765 )  == 0b10001000 );
		}
	}

	SECTION( "RL m" )
	{
		SECTION("RL A")
		{
			rlRegister( cpu, 0b10001000, 0b111 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetRegisterAF().hi == 0b00010000 );


			rlRegister( cpu, 0b01000100, 0b111 );
			check_flags( cpu, false, false, false, false );
			REQUIRE( cpu.GetRegisterAF().hi == 0b10001001 ); // 캐리라서 1 올라감.
		}

		SECTION("RL (HL)")
		{
			rlMemoryHL( cpu, 0b10001000, 0x342 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetMemoryValue( 0x342 ) == 0b00010000 );


			rlMemoryHL( cpu, 0b01000100, 0x342 );
			check_flags( cpu, false, false, false, false );
			REQUIRE( cpu.GetMemoryValue( 0x342 ) == 0b10001001 ); // 캐리라서 1..
		}
	}
}