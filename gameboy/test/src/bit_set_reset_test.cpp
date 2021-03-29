#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

TEST_CASE( "BIT SET RESET TEST", "[BIT SET RESET TEST]" )
{
	std::shared_ptr<GameboyCPU> ptr_cpu = GameboyCPU::Create();
    GameboyCPU & cpu = *(ptr_cpu);;
	cpu.TestReset();
	SECTION("BIT B, R")
	{
		SECTION("BIT 0~7, A")
		{
			callSetRegister8(cpu, Register8BitIndex::A, 0b10001000 );
			const bool RequireResult[8] = { true, true, true, false, true, true, true, false };

			for( int i = 0; i < 8; i++ )
			{
			 	REQUIRE( RequireResult[i] == bitTest(cpu, Register8BitIndex::A, i ) );
			}

			check_flags( cpu, false, true, false, false );
		}

		SECTION("BIT 0~7, (HL)")
		{
			callSetMemory3Step(cpu, Register8BitIndex::D, 0x3030, 0b10001000 );

			const bool RequireResult[8] = { true, true, true, false, true, true, true, false };

			for( int i = 0; i < 8; i++ )
			{
				REQUIRE( RequireResult[i] == bitTest(cpu, Register8BitIndex::MEM_HL, i ) );
			}

			check_flags( cpu, false, true, false, false );
		}
	}


	SECTION("SET B, R")
	{
		SECTION("SET 4, A")
		{
			callSetRegister8(cpu, Register8BitIndex::A, 0b0 );
			REQUIRE( 0b00010000 == setBitByRegister(cpu, Register8BitIndex::A, 4 ) );
		}

		SECTION("SET 7, (HL)")
		{
			callSetMemory3Step(cpu, Register8BitIndex::D, 0x2460, 0b0 );
			REQUIRE( 0b10000000 == setBitByMemory( cpu, 0x2460, 7 ) );
		}
	}


	SECTION("RES B, R")
	{
		SECTION("RES 6, D")
		{
			callSetRegister8(cpu, Register8BitIndex::D, 0xff );
			REQUIRE( 0b10111111 == resetBitByRegister(cpu, Register8BitIndex::D, 6 ) );
		}

		SECTION("RES 0, (HL)")
		{
			callSetMemory3Step(cpu, Register8BitIndex::E, 0x3080, 0xff );
			REQUIRE( 0b11111110 == resetBitByMemory( cpu, 0x3080, 0 ) );
		}
	}
}