#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

TEST_CASE("JUMP CODE POINT", "[JUMP]")
{
	GameboyCPU cpu;

	SECTION("JP (IF-COND) WORD")
	{
		SECTION("IF_COND = C, OK!")
		{
			subN( cpu, 0x0, 0x1 );
			check_flags( cpu, false, true, true, true );

			REQUIRE( 0x3000 == jumpToWordIfCarry( cpu, 0x3000 ));
		}

		SECTION("IF_COND = C, FAILED.")
		{
			subN( cpu, 0x2, 0x1 );
			check_flags( cpu, false, false, true, false );
			WORD expect_pc_value = cpu.GetRegisterPC().reg_16 + 3u; // JP, WORD_LO, WORD_HI

			REQUIRE( expect_pc_value == jumpToWordIfCarry( cpu, 0x3000 ));
		}

		SECTION("IF_COND = NC, OK!")
		{
			subN( cpu, 0x2, 0x1 );
			check_flags( cpu, false, false, true, false );

			REQUIRE( 0x3000 == jumpToWordIfNotCarry( cpu, 0x3000 ));
		}

		SECTION("IF_COND = NC, FAILED.")
		{
			subN( cpu, 0x0, 0x1 );
			check_flags( cpu, false, true, true, true );
			WORD expect_pc_value = cpu.GetRegisterPC().reg_16 + 3u; // JP, WORD_LO, WORD_HI

			REQUIRE( expect_pc_value == jumpToWordIfNotCarry( cpu, 0x3000 ));
		}

		SECTION("IF_COND = Z, OK!")
		{
			subN( cpu, 0x1, 0x1 );
			check_flags( cpu, true, false, true, false );

			REQUIRE( 0x3000 == jumpToWordIfZero( cpu, 0x3000 ));
		}

		SECTION("IF_COND = Z, FAILED.")
		{
			subN( cpu, 0x2, 0x1 );
			check_flags( cpu, false, false, true, false );
			WORD expect_pc_value = cpu.GetRegisterPC().reg_16 + 3u; // JP, WORD_LO, WORD_HI

			REQUIRE( expect_pc_value == jumpToWordIfZero( cpu, 0x3000 ));
		}

		SECTION("IF_COND = NZ, OK!")
		{
			subN( cpu, 0x2, 0x1 );
			check_flags( cpu, false, false, true, false );

			REQUIRE( 0x3000 == jumpToWordNotIfZero( cpu, 0x3000 ));
		}

		SECTION("IF_COND = NZ, FAILED.")
		{
			subN( cpu, 0x1, 0x1 );
			check_flags( cpu, true, false, true, false );
			WORD expect_pc_value = cpu.GetRegisterPC().reg_16 + 3u; // JP, WORD_LO, WORD_HI

			REQUIRE( expect_pc_value == jumpToWordNotIfZero( cpu, 0x3000 ));
		}
	}

}