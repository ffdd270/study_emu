//
// Created by HaruGakkaP on 2020-10-27.
//

#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

SCENARIO( "Gameboy CPU Test.", "[DEBUG]")
{
	GIVEN( "CPU, Logic." )
	{
		GameboyCPU cpu;

		constexpr WORD REQUIRE_BC_VALUE = 0x2401;
		constexpr WORD REQUIRE_A_VALUE = 0x24;
		constexpr WORD REQUIRE_DE_VALUE = 0x2411;
		constexpr WORD REQUIRE_H_VALUE = 0x2;

		setRegister16( cpu, Register16BitIndex::BC, 0x2401 ); // 0x1000~0x1003
		setRegister8( cpu, Param8BitIndex::A, 0x24 ); // 0x1004~0x1005
		setRegister16( cpu, Register16BitIndex::DE, 0x2411 ); // 0x1006~0x1009
		setRegister8( cpu, Param8BitIndex::H, 0x2 );

		WHEN("NO BP")
		{
			for( int i = 0; i < 4; i++ ) { cpu.NextStep(); }
			THEN( "BC = 0x2401, A = 0x24, DE = 0x2411, H = 0x2"  )
			{
				REQUIRE( cpu.GetRegisterBC().reg_16 == REQUIRE_BC_VALUE );
				REQUIRE( cpu.GetRegisterAF().hi == REQUIRE_A_VALUE );
				REQUIRE( cpu.GetRegisterDE().reg_16 == REQUIRE_DE_VALUE );
				REQUIRE( cpu.GetRegisterHL().hi == REQUIRE_H_VALUE );
			}
		}

		WHEN("BP 0x1000")
		{
			cpu.AddBreakPoint( 0x1000 );
			for( int i = 0; i < 4; i++ ) { cpu.NextStep(); }
		}
	}




}