//
// Created by HaruGakkaP on 2020-10-27.
//

#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

inline void ALL_CLEAR_CHECK( GameboyCPU & cpu, WORD REQUIRE_BC_VALUE, WORD REQUIRE_A_VALUE, WORD REQUIRE_DE_VALUE, WORD REQUIRE_H_VALUE )
{
	REQUIRE( cpu.GetRegisterBC().reg_16 == REQUIRE_BC_VALUE );
	REQUIRE( cpu.GetRegisterAF().hi == REQUIRE_A_VALUE );
	REQUIRE( cpu.GetRegisterDE().reg_16 == REQUIRE_DE_VALUE );
	REQUIRE( cpu.GetRegisterHL().hi == REQUIRE_H_VALUE );
}

SCENARIO( "Gameboy CPU Test.", "[DEBUG]")
{
	GIVEN( "CPU, Logic." )
	{
		GameboyCPU cpu;

		constexpr WORD REQUIRE_BC_VALUE = 0x2401;
		constexpr WORD REQUIRE_A_VALUE = 0x24;
		constexpr WORD REQUIRE_DE_VALUE = 0x2411;
		constexpr WORD REQUIRE_H_VALUE = 0x2;

		setRegister16( cpu, Register16BitIndex::BC, 0x2401 ); // 0x1000~0x1002
		setRegister8( cpu, Param8BitIndex::A, 0x24 ); // 0x1003~0x1004
		setRegister16( cpu, Register16BitIndex::DE, 0x2411 ); // 0x1005~0x1007
		setRegister8( cpu, Param8BitIndex::H, 0x2 ); // 0x1007~0x1008

		WHEN("NO BP")
		{
			for( int i = 0; i < 4; i++ ) { cpu.NextStep(); }
			THEN( "BC = 0x2401, A = 0x24, DE = 0x2411, H = 0x2"  )
			{
				ALL_CLEAR_CHECK( cpu, REQUIRE_BC_VALUE, REQUIRE_A_VALUE, REQUIRE_DE_VALUE, REQUIRE_H_VALUE );
			}
		}

		WHEN("BP 0x1000")
		{
			cpu.AddBreakPoint(0x1000);
			for (int i = 0; i < 4; i++){ cpu.NextStep(); }

			THEN("NOTHING CHANGED")
			{
				REQUIRE_FALSE( cpu.GetRegisterBC().reg_16 == REQUIRE_BC_VALUE );
				REQUIRE_FALSE( cpu.GetRegisterAF().hi == REQUIRE_A_VALUE );
				REQUIRE_FALSE( cpu.GetRegisterDE().reg_16 == REQUIRE_DE_VALUE );
				REQUIRE_FALSE( cpu.GetRegisterHL().hi == REQUIRE_H_VALUE );
			}
		}

		WHEN("BP 0x1004")
		{
			cpu.AddBreakPoint(0x1003);
			for (int i = 0; i < 4; i++){ cpu.NextStep(); }

			THEN("NOTHING CHANGED")
			{
				REQUIRE( cpu.GetRegisterBC().reg_16 == REQUIRE_BC_VALUE );
				REQUIRE_FALSE( cpu.GetRegisterAF().hi == REQUIRE_A_VALUE );
				REQUIRE_FALSE( cpu.GetRegisterDE().reg_16 == REQUIRE_DE_VALUE );
				REQUIRE_FALSE( cpu.GetRegisterHL().hi == REQUIRE_H_VALUE );
			}

			cpu.ContinueFromBreakPoint();
			for ( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			AND_THEN("Continue!")
			{
				ALL_CLEAR_CHECK( cpu, REQUIRE_BC_VALUE, REQUIRE_A_VALUE, REQUIRE_DE_VALUE, REQUIRE_H_VALUE );
			}
		}
	}
}