//
// Created by nhy20 on 2020-07-18.
//

#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

void NoFlagCheck( GameboyCPU & cpu )
{
	REQUIRE( cpu.GetFlagC() == 0 );
	REQUIRE( cpu.GetFlagH() == 0 );
	REQUIRE( cpu.GetFlagZ() == 0 );
	REQUIRE( cpu.GetFlagN() == 0 );
}

void ZeroFlagCheck( GameboyCPU & cpu )
{
	REQUIRE( cpu.GetFlagC() == 0 );
	REQUIRE( cpu.GetFlagH() == 0 );
	REQUIRE( cpu.GetFlagZ() == 1 );
	REQUIRE( cpu.GetFlagN() == 0 );
}

void CarryAndHalfFlagCheck(GameboyCPU & cpu )
{
	REQUIRE( cpu.GetFlagC() == 1 );
	REQUIRE( cpu.GetFlagH() == 1 );
	REQUIRE( cpu.GetFlagZ() == 0 );
	REQUIRE( cpu.GetFlagN() == 0 );
}

void HalfFlagCheck( GameboyCPU & cpu )
{
	REQUIRE( cpu.GetFlagC() == 0 );
	REQUIRE( cpu.GetFlagH() == 1 );
	REQUIRE( cpu.GetFlagZ() == 0 );
	REQUIRE( cpu.GetFlagN() == 0 );
}


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

			NoFlagCheck( cpu );

			setRegister8( cpu, 0b10, 0b1000000 ); // D = 0b1000000;
			// 1 Step.
			// Carry Flags Test.

			setRegister8( cpu, 0b111, 0b100 ); // A = 0b100;
			// 2 Step.
			// H Flags Test.

			cpu.InjectionMemory( 0b10000010 ); // ADD A, D
			// 3 Step. A = 0b01000100; H, C Flag On.

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			CarryAndHalfFlagCheck(cpu);

			setRegister8( cpu, 0b10, ~(static_cast<BYTE>(0b01000100)) + 0b1);
			// 0b10111011; + 1
			// 1 Step.

			cpu.InjectionMemory( 0b10000010 ); // ADD A, D
			// 오버플로우로 인한 비트 넘김. 0


			for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }

			ZeroFlagCheck( cpu );
		}
	}


	// 0b11000110 ( 0xC6 )
	SECTION("ADD A, imm8")
	{
		SECTION("ADD A, 0x24")
		{
			cpu.Reset();

			setRegister8( cpu, 0b111, 0x6 );
			// 1 Step.

			cpu.InjectionMemory( 0xC6 ); // ADD A, imm8
			cpu.InjectionMemory( 0x24 ); // imm value
			// 2 Step.

			for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }

			// 0x24 + 0x6 = 0x2A
			REQUIRE( cpu.GetRegisterAF().hi == 0x2A );
		}

		SECTION("Flags Test")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			setRegister8( cpu, 0b111, 0x0 );
			// 1 Step.

			cpu.InjectionMemory( 0xC6 );
			cpu.InjectionMemory( 0x00 );
			// 2 Step.

			for ( int i = 0; i < 2; i++ ) { cpu.NextStep(); }

			ZeroFlagCheck( cpu );

			cpu.InjectionMemory( 0xC6 );
			cpu.InjectionMemory( 0b00000100 );
			// 1 Step.

			cpu.NextStep();

			HalfFlagCheck( cpu );

			cpu.InjectionMemory( 0xC6 );
			cpu.InjectionMemory( 0b01000000 );
			// 1 Step.

			cpu.NextStep();

			CarryAndHalfFlagCheck( cpu );
		}
	}

	SECTION("ADD A, (HL)")
	{
		SECTION( "Add Test.")
		{
			cpu.Reset();

			setMemory3Step( cpu, 0, 0xBEEF, 0x30 );
			//HL = 0xBEEF;
			//B = 0x30;
			//0xBEEF = 0x30;
			// 3 Step.

			setRegister8( cpu, 0b111, 0xA );
			// A = 0xA
			// 4 Step.

			cpu.InjectionMemory( 0x86 ); // ADD A, (HL)/
			//5 Step.

			for( int i = 0; i < 5; i++ ) { cpu.NextStep(); }

			REQUIRE( cpu.GetRegisterAF().hi == 0x3A );
		}

		SECTION( "Flags Test.")
		{
			cpu.Reset();

			setMemory3Step( cpu, 0, 0xA0A0, 0x0 );
			// HL = 0xA0A0;
			// B = 0x0;
			// 0xA0A0 = 0;
			// 3 Step.

			setRegister8( cpu, 0b111, 0x0 );
			// A = 0x0
			// 4 Step.

			cpu.InjectionMemory( 0x86 ); // ADD A, (HL)/
			for ( int i = 0; i < 5; i++ ) { cpu.NextStep(); }

			ZeroFlagCheck( cpu );

			setMemory3Step( cpu, 0, 0xF0F0, 0b00000100 );
			// HL = 0xF0F0;
			// B = 0x0;
			// 0xF0F0 = 0b00000100;
			// 3 Step.

			cpu.InjectionMemory( 0x86 ); // ADD A, (HL)/
			for ( int i = 0; i < 4; i++ ) { cpu.NextStep(); }

			HalfFlagCheck( cpu );

			setMemory3Step( cpu, 0, 0xD0D0, 0b01000000 );
			// HL = 0xD0D0;
			// B = 0x0;
			// 0xD0D0 = 0b01000000;
			// 3 Step.

			cpu.InjectionMemory( 0x86 );
			for ( int i = 0; i < 4; i++ ) { cpu.NextStep(); }

			CarryAndHalfFlagCheck( cpu );
		}
	}
}