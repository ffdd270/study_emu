#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

inline void test_jr( GameboyCPU & cpu, char signed_value )
{
	WORD prv = cpu.GetRegisterPC().reg_16;

	cpu.InjectionMemory( 0x18 );
	cpu.InjectionMemory(signed_value );
	cpu.NextStep();

	REQUIRE( cpu.GetRegisterPC().reg_16 == prv + signed_value + 2 );
}

inline void test_jr_cond( GameboyCPU & cpu, BYTE op_code, char signed_value, bool cond )
{
	WORD prv = cpu.GetRegisterPC().reg_16;

	cpu.InjectionMemory( op_code );
	cpu.InjectionMemory( signed_value );
	cpu.NextStep();

	if ( cond )
	{
		REQUIRE( cpu.GetRegisterPC().reg_16 == prv + signed_value + 2 );
	}
	else
	{
		REQUIRE( cpu.GetRegisterPC().reg_16 == prv + 2 );
	}
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

	SECTION("JR cond signed_value")
	{
		SECTION("NZ")
		{
			SECTION("OK. singed_value = 0x10")
			{
				incR( cpu, Param8BitIndex::B, 0x0 ); // NZ;
				test_jr_cond( cpu, 0x20, 0x10, true );
			}

			SECTION("NOT OK")
			{
				incR( cpu, Param8BitIndex::B, 0xff ); // Z;
				test_jr_cond( cpu, 0x20, 0x10, false );
			}
		}

		SECTION("Z")
		{
			SECTION("OK. singed_value = -0x10")
			{
				incR( cpu, Param8BitIndex::B, 0xff ); // Z;
				test_jr_cond( cpu, 0x28, -0x10, true );
			}

			SECTION("NOT OK")
			{
				incR( cpu, Param8BitIndex::B, 0x0 ); // NZ;
				test_jr_cond( cpu, 0x28, -0x10, false );
			}
		}

		SECTION("NC")
		{
			SECTION("OK. singed_value = 0x30")
			{
				incR( cpu, Param8BitIndex::B, 0x0 ); // NC;
				test_jr_cond( cpu, 0x30, 0x30, true );
			}

			SECTION("NOT OK")
			{
				incR( cpu, Param8BitIndex::B, 0xff ); // C;
				test_jr_cond( cpu, 0x30, -0x30, false );
			}
		}

		SECTION("C")
		{
			SECTION("OK. singed_value = -0x30")
			{
				incR( cpu, Param8BitIndex::B, 0xff ); // C;
				test_jr_cond( cpu, 0x38, -0x30, true );
			}

			SECTION("NOT OK")
			{
				incR( cpu, Param8BitIndex::B, 0x0 ); // NC;
				test_jr_cond( cpu, 0x38, -0x30, false );
			}
		}
	}
}