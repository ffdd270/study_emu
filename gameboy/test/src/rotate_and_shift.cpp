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

	SECTION("RR m")
	{
		SECTION("RR A")
		{
			rrRegister( cpu, 0b00010001, 0b111 );
			check_flags(  cpu, false, false, false, true );
			REQUIRE( cpu.GetRegisterAF().hi == 0b00001000 );

			rrRegister( cpu, 0b00100010, 0b111 );
			check_flags(  cpu, false, false, false, false );
			REQUIRE( cpu.GetRegisterAF().hi == 0b10010001 );
		}


		SECTION("RR (HL)")
		{
			rrMemoryHL( cpu, 0b00010001, 0xf0f0 );
			check_flags(  cpu, false, false, false, true );
			REQUIRE( cpu.GetMemoryValue( 0xf0f0 ) == 0b00001000 );

			rrMemoryHL( cpu, 0b00100010, 0xf0f0 );
			check_flags(  cpu, false, false, false, false );
			REQUIRE( cpu.GetMemoryValue( 0xf0f0 ) == 0b10010001 );
		}
	}

	SECTION("SLA m")
	{
		SECTION("SLA A")
		{
			slaRegister( cpu, 0b10001000, 0b111 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetRegisterAF().hi == 0b00010000 );


			slaRegister( cpu, 0b01000100, 0b111 );
			check_flags( cpu, false, false, false, false );
			REQUIRE( cpu.GetRegisterAF().hi == 0b10001000 );
		}

		SECTION("SLA (HL)")
		{
			slaMemoryHL( cpu, 0b10001000, 0x342 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetMemoryValue( 0x342 ) == 0b00010000 );


			slaMemoryHL( cpu, 0b01000100, 0x342 );
			check_flags( cpu, false, false, false, false );
			REQUIRE( cpu.GetMemoryValue( 0x342 ) == 0b10001000 );
		}
	}

	SECTION("SRA m")
	{
		SECTION("SRA A")
		{
			sraRegister( cpu, 0b00010001, 0b111 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetRegisterAF().hi == 0b00001000 );


			sraRegister( cpu, 0b10001000, 0b111 );
			check_flags( cpu, false, false, false, false );
			REQUIRE( cpu.GetRegisterAF().hi == 0b11000100 ); // SRA는 이전값을 유지해준다.
		}


		SECTION("SRA (HL)")
		{
			sraMemoryHL( cpu, 0b00010001, 0x3030 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetMemoryValue(0x3030) == 0b00001000 );


			sraMemoryHL( cpu, 0b10001000, 0x3030 );
			check_flags( cpu, false, false, false, false );
			REQUIRE( cpu.GetMemoryValue(0x3030) == 0b11000100 ); // SRA는 이전값을 유지해준다.
		}
	}

	SECTION("SWAP m")
	{
		SECTION("SWAP B")
		 {
			swapRegister( cpu, 0b00101000u, Param8BitIndex::B );
			check_flags( cpu,  false, false, false, false );
			REQUIRE( cpu.GetRegisterBC().hi == 0b10000010u );

			swapRegister( cpu, 0b0, Param8BitIndex::B );
			check_flags( cpu,  true, false, false, false );
			REQUIRE( cpu.GetRegisterBC().hi == 0 );
		}

		SECTION("SWAP (HL)")
		{
			swapMemoryHL( cpu, 0b00101000u, 0x3024 );
			check_flags( cpu,  false, false, false, false );
			REQUIRE( cpu.GetMemoryValue(0x3024) == 0b10000010u );

			swapMemoryHL( cpu, 0b0, 0x3024 );
			check_flags( cpu,  true, false, false, false );
			REQUIRE( cpu.GetMemoryValue(0x3024) == 0 );
		}
	}

	SECTION("SRL m")
	{
		SECTION("SRL A")
		{
			srlRegister( cpu, 0b00010001, 0b111 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetRegisterAF().hi == 0b00001000 );


			srlRegister( cpu, 0b10001000, 0b111 );
			check_flags( cpu, false, false, false, false );
			REQUIRE( cpu.GetRegisterAF().hi == 0b01000100 );
		}

		SECTION("SRL (HL)")
		{
			srlMemoryHL( cpu, 0b00010001, 0x3030 );
			check_flags( cpu, false, false, false, true );
			REQUIRE( cpu.GetMemoryValue(0x3030) == 0b00001000 );


			srlMemoryHL( cpu, 0b10001000, 0x3030 );
			check_flags( cpu, false, false, false, false );
			REQUIRE( cpu.GetMemoryValue(0x3030) == 0b01000100 );
		}
	}

	SECTION("RRCA")
	{
		rrcRegisterA( cpu, 0b00010001 );
		check_flags( cpu, false, false, false, true );
		REQUIRE( cpu.GetRegisterAF().hi == 0b10001000 );

		rrcRegisterA( cpu, 0b00001000 );
		check_flags( cpu, false, false, false, false );
		REQUIRE( cpu.GetRegisterAF().hi == 0b00000100 );
	}

	SECTION("RRA")
	{
		rrRegisterA( cpu, 0b10000001 );
		check_flags( cpu, false, false, false, true );
		REQUIRE( cpu.GetRegisterAF().hi == 0b01000000 );

		rrRegisterA( cpu, 0b00001000 );
		check_flags( cpu, false, false, false, false );
		REQUIRE( cpu.GetRegisterAF().hi == 0b10000100 );
	}
}