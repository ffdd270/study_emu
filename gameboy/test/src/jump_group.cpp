#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

inline void test_jr( GameboyCPU & cpu, char signed_value )
{
	signed_value = -80;

	WORD prv = cpu.GetRegisterPC().reg_16;

	cpu.InjectionMemory( 0x18 );
	cpu.InjectionMemory(signed_value );
	for( size_t i = 0; i < 2; i++ ) { cpu.NextStep(); }

	REQUIRE( cpu.GetRegisterPC().reg_16 == prv + signed_value + 2 );
}

TEST_CASE("JUMP CODE POINT", "[JUMP]")
{
	std::shared_ptr<GameboyCPU> ptr_cpu = GameboyCPU::Create();
    GameboyCPU & cpu = *(ptr_cpu);;

	SECTION("JP (IF-COND) WORD")
	{
		SECTION("IF_COND = C, OK!")
		{
			call_subN(cpu, 0x0, 0x1);
			check_flags( cpu, false, true, true, true );

			REQUIRE( 0x3000 == jumpToWordIfCarry( cpu, 0x3000 ));
		}

		SECTION("IF_COND = C, FAILED.")
		{
			call_subN(cpu, 0x2, 0x1);
			check_flags( cpu, false, false, true, false );
			WORD expect_pc_value = cpu.GetRegisterPC().reg_16 + 3u; // JP, WORD_LO, WORD_HI

			REQUIRE( expect_pc_value == jumpToWordIfCarry( cpu, 0x3000 ));
		}

		SECTION("IF_COND = NC, OK!")
		{
			call_subN(cpu, 0x2, 0x1);
			check_flags( cpu, false, false, true, false );

			REQUIRE( 0x3000 == jumpToWordIfNotCarry( cpu, 0x3000 ));
		}

		SECTION("IF_COND = NC, FAILED.")
		{
			call_subN(cpu, 0x0, 0x1);
			check_flags( cpu, false, true, true, true );
			WORD expect_pc_value = cpu.GetRegisterPC().reg_16 + 3u; // JP, WORD_LO, WORD_HI

			REQUIRE( expect_pc_value == jumpToWordIfNotCarry( cpu, 0x3000 ));
		}

		SECTION("IF_COND = Z, OK!")
		{
			call_subN(cpu, 0x1, 0x1);
			check_flags( cpu, true, false, true, false );

			REQUIRE( 0x3000 == jumpToWordIfZero( cpu, 0x3000 ));
		}

		SECTION("IF_COND = Z, FAILED.")
		{
			call_subN(cpu, 0x2, 0x1);
			check_flags( cpu, false, false, true, false );
			WORD expect_pc_value = cpu.GetRegisterPC().reg_16 + 3u; // JP, WORD_LO, WORD_HI

			REQUIRE( expect_pc_value == jumpToWordIfZero( cpu, 0x3000 ));
		}

		SECTION("IF_COND = NZ, OK!")
		{
			call_subN(cpu, 0x2, 0x1);
			check_flags( cpu, false, false, true, false );

			REQUIRE( 0x3000 == jumpToWordNotIfZero( cpu, 0x3000 ));
		}

		SECTION("IF_COND = NZ, FAILED.")
		{
			call_subN(cpu, 0x1, 0x1);
			check_flags( cpu, true, false, true, false );
			WORD expect_pc_value = cpu.GetRegisterPC().reg_16 + 3u; // JP, WORD_LO, WORD_HI

			REQUIRE( expect_pc_value == jumpToWordNotIfZero( cpu, 0x3000 ));
		}
	}

	SECTION("JP WORD")
	{
		REQUIRE( 0x3000 == jumpToWord( cpu, 0x3000 ) );
	}

	SECTION("JP HL")
	{
		REQUIRE( 0x4242 == jumpToHL( cpu, 0x4242 ) );
	}

	SECTION("JR signed_value")
	{
		SECTION("signed_value = 125")
		{
			test_jr( cpu, 125 );
		}

		SECTION("signed_value = -80")
		{
			test_jr( cpu, -80 );
		}
	}
}