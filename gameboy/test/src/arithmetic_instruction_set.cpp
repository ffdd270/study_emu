//
// Created by nhy20 on 2020-07-18.
//

#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"


TEST_CASE( "ARITHMETIC INSTRUCTION", "[Math]")
{
	GameboyCPU cpu;

	SECTION("ADD A, register")
	{
		SECTION("register = D")
		{
			cpu.Reset();

			setRegister8( cpu, 0b10, 0x21 );   // D = 0x21
			// 1 Step.

			setRegister8( cpu, 0b111, 0x04 );  // A = 0x04
			// 2 Step.

			cpu.InjectionMemory( 0b10000010 ); // ADD A, D
			// 3 Step. A = 0x04 + 0x21 = 0x25.

			for ( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			REQUIRE( cpu.GetRegisterAF().hi == 0x25 );
		}
	}
}