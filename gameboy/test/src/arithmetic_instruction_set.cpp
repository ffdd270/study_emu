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

		SECTION("Flag Check.")
		{
			cpu.Reset();

			REQUIRE( cpu.GetFlagC() == 0 );
			REQUIRE( cpu.GetFlagZ() == 0 );
			REQUIRE( cpu.GetFlagH() == 0 );
			REQUIRE( cpu.GetFlagN() == 0 );


			setRegister8( cpu, 0b10, 0b1000000 ); // D = 0b1000000;
			// 1 Step.
			// Carry Flags Test.

			setRegister8( cpu, 0b111, 0b100 ); // A = 0b100;
			// 2 Step.
			// H Flags Test.

			cpu.InjectionMemory( 0b10000010 ); // ADD A, D
			// 3 Step. A = 0b01000100; H, C Flag On.

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			REQUIRE( cpu.GetFlagC() == 1 );
			REQUIRE( cpu.GetFlagH() == 1 );
			REQUIRE( cpu.GetFlagZ() == 0 );
			REQUIRE( cpu.GetFlagN() == 0 );


			setRegister8( cpu, 0b10, ~(static_cast<BYTE>(0b01000100)) + 0b1);
			// 0b10111011; + 1
			// 1 Step.

			cpu.InjectionMemory( 0b10000010 ); // ADD A, D
			// 오버플로우로 인한 비트 넘김. 0


			for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }

			REQUIRE( cpu.GetFlagC() == 0 );
			REQUIRE( cpu.GetFlagH() == 0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
			REQUIRE( cpu.GetFlagN() == 0 );

		}
	}
}