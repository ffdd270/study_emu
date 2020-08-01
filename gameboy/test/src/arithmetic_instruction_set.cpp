//
// Created by nhy20 on 2020-07-18.
//

#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"


// 0b11010110 (0xD6)
void subN( GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	setRegister8( cpu, 0b111, a_value );
	cpu.InjectionMemory( 0xD6 );
	cpu.InjectionMemory( n );

	for ( int i = 0; i < 2; i++ ) { cpu.NextStep(); }
}

// 0b10010110 (0x96)
void subHL(  GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	setMemory3Step( cpu, 0b100, mem_hl_address, n );
	setRegister8( cpu, 0b111, a_value );
	cpu.InjectionMemory( 0x96 ); // SUB (HL)

	for( int i = 0; i < 5; i++ ) { cpu.NextStep(); }
}


void NoFlagCheck( GameboyCPU & cpu )
{
	REQUIRE( cpu.GetFlagC() == 0 );
	REQUIRE( cpu.GetFlagH() == 0 );
	REQUIRE( cpu.GetFlagZ() == 0 );
	REQUIRE( cpu.GetFlagN() == 0 );
}

void CarryAndZeroFlagCheck( GameboyCPU & cpu )
{
	REQUIRE( cpu.GetFlagC() == 1 );
	REQUIRE( cpu.GetFlagH() == 0 );
	REQUIRE( cpu.GetFlagZ() == 1 );
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

void MemHLFlagTest( GameboyCPU & cpu )
{
	cpu.Reset();

	NoFlagCheck( cpu );
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

	setRegister8( cpu, 0b111, 0x08 ); // Half + hafl
	setMemory3Step( cpu, 0, 0xF0F0, 0x08 );
	// HL = 0xF0F0;
	// B = 0x0;
	// 0xF0F0 = 0b00000100;
	// 3 Step.

	cpu.InjectionMemory( 0x86 ); // ADD A, (HL)/
	for ( int i = 0; i < 5; i++ ) { cpu.NextStep(); }

	HalfFlagCheck( cpu );

	setRegister8( cpu, 0b111, 0x88 );
	setMemory3Step( cpu, 0, 0xD0D0, 0x88 );
	// HL = 0xD0D0;
	// B = 0x0;
	// 0xD0D0 = 0b01000000;
	// 3 Step.

	cpu.InjectionMemory( 0x86 );
	for ( int i = 0; i < 5; i++ ) { cpu.NextStep(); }

	CarryAndHalfFlagCheck( cpu );
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

			setRegister8( cpu, 0b10, 0b10001000 ); // D = 0b10000000;
			// 1 Step.
			// Carry - Half Flags Test.

			setRegister8( cpu, 0b111, 0b10001000 ); // A = 0b1000;
			// 2 Step.
			//  0b1000 + 0b1000 = 0b10000 == Half
			//  0b10000000 + 0b10000000 = 0x100 == Carry

			cpu.InjectionMemory( 0b10000010 ); // ADD A, D
			// 3 Step. A = 0b100010000 == (8Bit) 0b00010000. H, C Flag On.

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			CarryAndHalfFlagCheck(cpu);

			setRegister8( cpu, 0b10, ~(static_cast<BYTE>(0b00010000)) + 0b1 );
			// 0b10111011; + 1
			// 1 Step.

			cpu.InjectionMemory( 0b10000010 ); // ADD A, D
			// 오버플로우로 인한 비트 넘김. 0


			for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }

			CarryAndZeroFlagCheck( cpu );
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

			// A = 0
			setRegister8( cpu, 0b111, 0b1000 ); // Half Carry
			// A = 0x08
			// 1 Step.
			cpu.InjectionMemory( 0xC6 );
			cpu.InjectionMemory( 0b00001000 );
			// 2 Step.

			for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }

			HalfFlagCheck( cpu );
			// A = 0x08.
			setRegister8( cpu, 0b111, 0x88 ); // HALF AND CARRY
			// A = 0x88

			cpu.InjectionMemory( 0xC6 );
			cpu.InjectionMemory(  0x88 );
			// 1 Step.

			for( int i = 0; i < 2; i++ ) {  cpu.NextStep(); }

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
			MemHLFlagTest( cpu );
		}
	}

	// 0b10001rrr (r = m8BitArguments)
	SECTION("ADC A, s")
	{
		SECTION( "Add Test" )
		{
			cpu.Reset();

			setRegister8(cpu, 0b10, 0x80);   // D = 0b10000000
			// 1 Step.

			setRegister8(cpu, 0b111, 0x80);  // A = 0x80
			// 2 Step.

			cpu.InjectionMemory(0b10000010); // ADD A, D
			// 3 Step. A = 0 + 0b10000000 ;

			for ( int i = 0; i < 3; i++ ) { cpu.NextStep(); }
			// Carry.
			REQUIRE( cpu.GetFlagC() == 1 );
			int prv_value = cpu.GetRegisterAF().hi;

			setRegister8( cpu, 0b11, 0xA ); // E = A
			// 1 Step.

			cpu.InjectionMemory( 0b10001011 ); // ADC A, E
			// 2 Step. A = 0b10000000 + 0xA + Carry;

			for ( int i = 0; i < 2; i++ ) { cpu.NextStep(); }

			REQUIRE( ( prv_value + 0xA + 1 ) == cpu.GetRegisterAF().hi );
		}

		SECTION("Flag Test.")
		{
			cpu.Reset();
			BYTE ADC_A_D = 0b10001010;

			NoFlagCheck( cpu );

			setRegister8( cpu, 0b10, 0 ); // D = 0;
			setRegister8( cpu, 0b111, 0 ); // A = 0;
			cpu.InjectionMemory( ADC_A_D ); // ADC A, D

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }
			ZeroFlagCheck( cpu );

			setRegister8( cpu, 0b111, 0x08 );
			setRegister8( cpu, 0b10, 0x08 ); // D = Half;
			cpu.InjectionMemory( ADC_A_D ); // ADC A, D.A = 0b1000

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }
			HalfFlagCheck( cpu );

			setRegister8( cpu, 0b111, 0x88);
			setRegister8( cpu, 0b10, 0x88 ); // D = Carry;
			cpu.InjectionMemory( ADC_A_D );
			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			CarryAndHalfFlagCheck( cpu );
		}
	}

	// 0b11001110 ( 0xCE )
	SECTION("ADC A, n")
	{
		SECTION("ADD TEST")
		{
			cpu.Reset();

			setRegister8( cpu, 0b111, 0x80 );
			// 1 Step.

			cpu.InjectionMemory( 0xCE ); // ADD A, imm8
			cpu.InjectionMemory( 0x80 ); // imm value ( Carry )
			// 2 Step.

			for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }

			// 0b1 + 0b10000000
			REQUIRE( cpu.GetRegisterAF().hi == 0 );
			REQUIRE( cpu.GetFlagC() == 1 );

			cpu.InjectionMemory( 0xCE );
			cpu.InjectionMemory( 0b0 );

			cpu.NextStep();
			// Carry Added
			REQUIRE( cpu.GetRegisterAF().hi == 1 );
		}

		SECTION("FLAGS TEST")
		{
			cpu.Reset();
			NoFlagCheck(  cpu );

			setRegister8( cpu, 0b111, 0 );

			cpu.InjectionMemory(0xCE);
			cpu.InjectionMemory( 0x00 ); // Zero

			for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }
			ZeroFlagCheck( cpu );

			setRegister8( cpu, 0b111, 0x08 );

			cpu.InjectionMemory( 0xCE );
			cpu.InjectionMemory( 0x08 ); // Half
			for (int i = 0; i < 2; i++ ) { cpu.NextStep(); }
			HalfFlagCheck( cpu );

			setRegister8( cpu, 0b111, 0x88 );
			cpu.InjectionMemory( 0xCE );
			cpu.InjectionMemory( 0x88 ); // Carry
			for (int i = 0; i < 2; i++ ) { cpu.NextStep(); }

			CarryAndHalfFlagCheck( cpu );
		}
	}

	// 0b10001110
	SECTION("ADC A, (HL)")
	{
		SECTION("ADD TEST")
		{
			cpu.Reset();
			setMemory3Step( cpu, 0b0, 0x0765, 0x8B );
			// HL = 0x765, B = 0xA,  (0x765) = 0x80 ( Carry )

			setRegister8( cpu, 0b111, 0x80 );
			// A = 0xB

			cpu.InjectionMemory( 0b10001110 ); // A = 0x80 + 0xB

			for( int i = 0; i < 5; i++ ) { cpu.NextStep(); }

			REQUIRE( cpu.GetRegisterAF().hi == 0xB );
			REQUIRE( cpu.GetFlagC() == 1 );

			setMemory3Step( cpu, 0b0, 0x765, 0 );
			// HL = 0x765, B = 2, (0x765) = 2.

			cpu.InjectionMemory( 0b10001110 ); // A = 0x8B + 2 + ( Carry )

			for ( int i = 0; i < 4; i++ ) { cpu.NextStep(); }
			REQUIRE( cpu.GetRegisterAF().hi == 0xC );
		}

		SECTION("Flag Test")
		{
			MemHLFlagTest( cpu );
		}
	}


	// 0b10010rrr { r = m8BitArguments }
	SECTION("SUB r")
	{
		SECTION("SUB TEST")
		{
			cpu.Reset();

			setRegister8( cpu, 0b111,  0xA );
			setRegister8( cpu, 0, 0xA );
			// 2 Step.

			cpu.InjectionMemory( 0b10010000 ); // SUB B
			// 3 Step.

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			REQUIRE( cpu.GetRegisterAF().hi == 0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}

		SECTION("FLAG TEST")
		{
			cpu.Reset();

			setRegister8( cpu, 0b111, 0xA8 );
			setRegister8( cpu, 0b0, 0x99 );
			cpu.InjectionMemory( 0b10010000 ); // SUB B
			// 3 Step.

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			REQUIRE( cpu.GetRegisterAF().hi == 0xF );
			REQUIRE( cpu.GetFlagH() == 1 );

			setRegister8( cpu, 0b111, 0xA0 );
			setRegister8( cpu, 0b10, 0xA1 );
			cpu.InjectionMemory( 0b10010010 ); // SUB D
			// 3 Step.

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			REQUIRE( cpu.GetRegisterAF().hi == 0xff ); // Underflow.
			REQUIRE( cpu.GetFlagC() == 1 );
			REQUIRE( cpu.GetFlagH() == 1 );

			setRegister8( cpu, 0b111, 0xB0 );
			setRegister8( cpu, 0b11, 0xB0 );
			cpu.InjectionMemory( 0b10010011 ); // SUB E

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			REQUIRE( cpu.GetRegisterAF().hi == 0x0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}
	}

	// 0b11010110 (0xD6)
	SECTION( "SUB N" )
	{
		SECTION( "SUB TEST" )
		{
			cpu.Reset();
			subN( cpu, 0x30, 0x28 );
			REQUIRE( cpu.GetRegisterAF().hi == 0x8 );

			subN( cpu, 0x30, 0x31 ); // underflow test.
			REQUIRE( cpu.GetRegisterAF().hi == 0xff );
		}

		SECTION( "FLAG TEST" )
		{
			cpu.Reset();

			subN( cpu, 0x35, 0x31 );
			REQUIRE( cpu.GetRegisterAF().hi == 0x4 );
			NoFlagCheck( cpu );

			subN( cpu, 0x31, 0x31 ); // Z Flag.
			REQUIRE( cpu.GetRegisterAF().hi == 0x0 );
			REQUIRE( cpu.GetFlagZ() == 1 );

			subN( cpu, 0x38, 0x2A ); // H Flag.
			REQUIRE( cpu.GetRegisterAF().hi == 0xE );
			REQUIRE( cpu.GetFlagH() == 1 );

			subN( cpu, 0x2A, 0x30 ); // C Flag.
			REQUIRE( cpu.GetRegisterAF().hi == ( 0xff - 5 ) );
			REQUIRE( cpu.GetFlagC() == 1 );
		}
	}

	SECTION( "SUB (HL)")
	{
		SECTION( "SUB TEST" )
		{
			cpu.Reset();
			subHL( cpu, 0xff, 0x3000, 0xfe );
			REQUIRE( cpu.GetRegisterAF().hi == 0x1 );
		}

		SECTION( "FLAG TEST" )
		{
			cpu.Reset();
			subHL( cpu, 0xfb, 0x3000, 0xea ); // NO FLAG.
			REQUIRE( cpu.GetRegisterAF().hi == 0x11 );
			NoFlagCheck( cpu );

			subHL( cpu, 0xff, 0x3530, 0xff ); // Z FLAG.
			REQUIRE( cpu.GetRegisterAF().hi == 0x0 );
			REQUIRE( cpu.GetFlagZ() == 1 );

			subHL( cpu, 0x3a, 0x4567, 0x2b ); // H FLAG.
			REQUIRE( cpu.GetRegisterAF().hi == 0xf );
			REQUIRE( cpu.GetFlagH() == 1 );

			subHL( cpu, 0x3b, 0x4252, 0x3c ); // C Flag.
			REQUIRE( cpu.GetRegisterAF().hi == 0xff );
			REQUIRE( cpu.GetFlagC() == 1 );
		}
	}
}