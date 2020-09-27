#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

TEST_CASE( "BIT SET RESET TEST", "[BIT SET RESET TEST]" )
{
	GameboyCPU cpu;

	SECTION("BIT B, R")
	{
		SECTION("BIT 0~7, A")
		{
			callSetRegister8( cpu, Param8BitIndex::A, 0b10001000 );
			const bool RequireResult[8] = { true, true, true, false, true, true, true, false };

			for( int i = 0; i < 8; i++ )
			{
			 	REQUIRE( RequireResult[i] == bitTest( cpu, Param8BitIndex::A,  i ) );
			}

			check_flags( cpu, false, true, false, false );
		}

		SECTION("BIT 0~7, (HL)")
		{
			callSetMemory3Step( cpu, Param8BitIndex::D, 0x3030, 0b10001000 );

			const bool RequireResult[8] = { true, true, true, false, true, true, true, false };

			for( int i = 0; i < 8; i++ )
			{
				REQUIRE( RequireResult[i] == bitTest( cpu, Param8BitIndex::MEM_HL, i ) );
			}

			check_flags( cpu, false, true, false, false );
		}
	}

}