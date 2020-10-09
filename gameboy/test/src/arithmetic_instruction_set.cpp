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

	setRegister8(cpu, Param8BitIndex::A, 0x0 );
	// A = 0x0
	// 4 Step.

	cpu.InjectionMemory( 0x86 ); // ADD A, (HL)/
	for ( int i = 0; i < 5; i++ ) { cpu.NextStep(); }

	ZeroFlagCheck( cpu );

	setRegister8(cpu, Param8BitIndex::A, 0x08 ); // Half + hafl
	setMemory3Step( cpu, 0, 0xF0F0, 0x08 );
	// HL = 0xF0F0;
	// B = 0x0;
	// 0xF0F0 = 0b00000100;
	// 3 Step.

	cpu.InjectionMemory( 0x86 ); // ADD A, (HL)/
	for ( int i = 0; i < 5; i++ ) { cpu.NextStep(); }

	HalfFlagCheck( cpu );

	setRegister8(cpu, Param8BitIndex::A, 0x88 );
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

			setRegister8(cpu, Param8BitIndex::D, 0x21 );   // D = 0x21
			// 1 Step.

			setRegister8(cpu, Param8BitIndex::A, 0x04 );  // A = 0x04
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

			setRegister8(cpu, Param8BitIndex::D, 0b10001000 ); // D = 0b10000000;
			// 1 Step.
			// Carry - Half Flags Test.

			setRegister8(cpu, Param8BitIndex::A, 0b10001000 ); // A = 0b1000;
			// 2 Step.
			//  0b1000 + 0b1000 = 0b10000 == Half
			//  0b10000000 + 0b10000000 = 0x100 == Carry

			cpu.InjectionMemory( 0b10000010 ); // ADD A, D
			// 3 Step. A = 0b100010000 == (8Bit) 0b00010000. H, C Flag On.

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			CarryAndHalfFlagCheck(cpu);

			setRegister8(cpu, Param8BitIndex::D, ~(static_cast<BYTE>(0b00010000)) + 0b1 );
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

			setRegister8(cpu, Param8BitIndex::A, 0x6 );
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

			setRegister8(cpu, Param8BitIndex::A, 0x0 );
			// 1 Step.

			cpu.InjectionMemory( 0xC6 );
			cpu.InjectionMemory( 0x00 );
			// 2 Step.

			for ( int i = 0; i < 2; i++ ) { cpu.NextStep(); }

			ZeroFlagCheck( cpu );

			// A = 0
			setRegister8(cpu, Param8BitIndex::A, 0b1000 ); // Half Carry
			// A = 0x08
			// 1 Step.
			cpu.InjectionMemory( 0xC6 );
			cpu.InjectionMemory( 0b00001000 );
			// 2 Step.

			for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }

			HalfFlagCheck( cpu );
			// A = 0x08.
			setRegister8(cpu, Param8BitIndex::A, 0x88 ); // HALF AND CARRY
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

			addAtoHL( cpu, 0xBEEF, 0x30, 0xA );
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

			setRegister8(cpu, Param8BitIndex::D, 0x80);   // D = 0b10000000
			// 1 Step.

			setRegister8(cpu, Param8BitIndex::A, 0x80);  // A = 0x80
			// 2 Step.

			cpu.InjectionMemory(0b10000010); // ADD A, D
			// 3 Step. A = 0 + 0b10000000 ;

			for ( int i = 0; i < 3; i++ ) { cpu.NextStep(); }
			// Carry.
			REQUIRE( cpu.GetFlagC() == 1 );
			int prv_value = cpu.GetRegisterAF().hi;

			setRegister8(cpu, Param8BitIndex::E, 0xA ); // E = A
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

			setRegister8(cpu, Param8BitIndex::D, 0 ); // D = 0;
			setRegister8(cpu, Param8BitIndex::A, 0 ); // A = 0;
			cpu.InjectionMemory( ADC_A_D ); // ADC A, D

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }
			ZeroFlagCheck( cpu );

			setRegister8(cpu, Param8BitIndex::A, 0x08 );
			setRegister8(cpu, Param8BitIndex::D, 0x08 ); // D = Half;
			cpu.InjectionMemory( ADC_A_D ); // ADC A, D.A = 0b1000

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }
			HalfFlagCheck( cpu );

			setRegister8(cpu, Param8BitIndex::A, 0x88);
			setRegister8(cpu, Param8BitIndex::D, 0x88 ); // D = Carry;
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

			setRegister8(cpu, Param8BitIndex::A, 0x80 );
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

			setRegister8(cpu, Param8BitIndex::A, 0 );

			cpu.InjectionMemory(0xCE);
			cpu.InjectionMemory( 0x00 ); // Zero

			for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }
			ZeroFlagCheck( cpu );

			setRegister8(cpu, Param8BitIndex::A, 0x08 );

			cpu.InjectionMemory( 0xCE );
			cpu.InjectionMemory( 0x08 ); // Half
			for (int i = 0; i < 2; i++ ) { cpu.NextStep(); }
			HalfFlagCheck( cpu );

			setRegister8(cpu, Param8BitIndex::A, 0x88 );
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
			setMemory3Step(cpu, Param8BitIndex::B, 0x0765, 0x8B );
			// HL = 0x765, B = 0xA,  (0x765) = 0x80 ( Carry )

			setRegister8(cpu, Param8BitIndex::A, 0x80 );
			// A = 0xB

			cpu.InjectionMemory( 0b10001110 ); // A = 0x80 + 0xB

			for( int i = 0; i < 5; i++ ) { cpu.NextStep(); }

			REQUIRE( cpu.GetRegisterAF().hi == 0xB );
			REQUIRE( cpu.GetFlagC() == 1 );

			setMemory3Step(cpu, Param8BitIndex::B, 0x765, 0 );
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

			setRegister8(cpu, Param8BitIndex::A, 0xA );
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

			setRegister8(cpu, Param8BitIndex::A, 0xA8 );
			setRegister8(cpu, Param8BitIndex::B, 0x99 );
			cpu.InjectionMemory( 0b10010000 ); // SUB B
			// 3 Step.

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			REQUIRE( cpu.GetRegisterAF().hi == 0xF );
			REQUIRE( cpu.GetFlagH() == 1 );

			setRegister8(cpu, Param8BitIndex::A, 0xA0 );
			setRegister8(cpu, Param8BitIndex::D, 0xA1 );
			cpu.InjectionMemory( 0b10010010 ); // SUB D
			// 3 Step.

			for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

			REQUIRE( cpu.GetRegisterAF().hi == 0xff ); // Underflow.
			REQUIRE( cpu.GetFlagC() == 1 );
			REQUIRE( cpu.GetFlagH() == 1 );

			setRegister8(cpu, Param8BitIndex::A, 0xB0 );
			setRegister8(cpu, Param8BitIndex::E, 0xB0 );
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
			call_subN(cpu, 0x30, 0x28);
			REQUIRE( cpu.GetRegisterAF().hi == 0x8 );

			call_subN(cpu, 0x30, 0x31); // underflow test.
			REQUIRE( cpu.GetRegisterAF().hi == 0xff );
		}

		SECTION( "FLAG TEST" )
		{
			cpu.Reset();

			call_subN(cpu, 0x35, 0x31);
			REQUIRE( cpu.GetRegisterAF().hi == 0x4 );
			check_flags( cpu, false, false, true, false );

			call_subN(cpu, 0x31, 0x31); // Z Flag.
			REQUIRE( cpu.GetRegisterAF().hi == 0x0 );
			check_flags( cpu, true, false, true, false );

			call_subN(cpu, 0x38, 0x2A); // H Flag.
			REQUIRE( cpu.GetRegisterAF().hi == 0xE );
			check_flags( cpu, false, true, true, false );

			call_subN(cpu, 0x2A, 0x30); // C Flag.
			REQUIRE( cpu.GetRegisterAF().hi == ( 0xff - 5 ) );
			check_flags( cpu, false, false, true, true );
		}
	}

	SECTION( "SUB (HL)")
	{
		SECTION( "SUB TEST" )
		{
			cpu.Reset();
			call_subHL(cpu, 0xff, 0x3000, 0xfe);
			REQUIRE( cpu.GetRegisterAF().hi == 0x1 );
		}

		SECTION( "FLAG TEST" )
		{
			cpu.Reset();
			call_subHL(cpu, 0xfb, 0x3000, 0xea); // NO FLAG.
			REQUIRE( cpu.GetRegisterAF().hi == 0x11 );
			check_flags( cpu, false, false, true, false );

			call_subHL(cpu, 0xff, 0x3530, 0xff); // Z FLAG.
			REQUIRE( cpu.GetRegisterAF().hi == 0x0 );
			check_flags( cpu, true, false, true, false );

			call_subHL(cpu, 0x3a, 0x4567, 0x2b); // H FLAG.
			REQUIRE( cpu.GetRegisterAF().hi == 0xf );
			check_flags( cpu, false, true, true, false );

			call_subHL(cpu, 0x3b, 0x4252, 0x3c); // C Flag.
			REQUIRE( cpu.GetRegisterAF().hi == 0xff );
			check_flags( cpu, false, true, true, true );
		}
	}

	SECTION( "SBC A, reg" )
	{
		SECTION( "SUB TEST" )
		{
			cpu.Reset();

			call_subRC(cpu, 0x30, 0x31);
			// Carry.
			REQUIRE( cpu.GetRegisterAF().hi == 0xff );
			REQUIRE( cpu.GetFlagC() == 1 );
			REQUIRE( cpu.GetFlagH() == 1 );

			call_subRC(cpu, 0x1A, 0x19);
			// With Carry SUB .
			REQUIRE( cpu.GetRegisterAF().hi == 0x0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}
	}

	SECTION( "SBC A, n" )
	{
		SECTION("SUB TEST")
		{
			cpu.Reset();

			call_subNC(cpu, 0x30, 0x31);
			// Carry.
			REQUIRE( cpu.GetRegisterAF().hi == 0xff );
			REQUIRE( cpu.GetFlagC() == 1 );

			call_subNC(cpu, 0x06, 0x05);
			// With Carry SUB.  -1 plus.
			REQUIRE( cpu.GetRegisterAF().hi == 0x0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}


		SECTION("FLAG TEST")
		{
			cpu.Reset();

			call_subNC(cpu, 0x41, 0x45); // Underflow 4.
			REQUIRE( cpu.GetRegisterAF().hi == ( 0xff - 3 ) );
			REQUIRE( cpu.GetFlagC() == 1 );
			REQUIRE( cpu.GetFlagH() == 1 );

			call_subNC(cpu, 0x53, 0x43);
			// With Carry.
			REQUIRE( cpu.GetRegisterAF().hi == 0xF );
			REQUIRE( cpu.GetFlagH() == 1 );
			REQUIRE( cpu.GetFlagC() == 0 );

			call_subNC(cpu, 0x42, 0x42);
			REQUIRE( cpu.GetRegisterAF().hi == 0x0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}
	}

	SECTION("SBC A, (HL)")
	{
		SECTION("SUB TEST")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_subHLC(cpu, 0xf0, 0xf0f0, 0xf1);
			// Carry .
			REQUIRE( cpu.GetRegisterHL().reg_16 == 0xf0f0 );
			REQUIRE( cpu.GetRegisterAF().hi == 0xff );
			check_flags( cpu, false, true, true, true );

			call_subHLC(cpu, 0x32, 0xf2f2, 0x30);
			//With Carry.
			REQUIRE( cpu.GetRegisterHL().reg_16 == 0xf2f2 );
			REQUIRE( cpu.GetRegisterAF().hi == 0x1 );
			check_flags( cpu, false, false, true, false );

			call_subHLC(cpu, 0x31, 0xfefe, 0x2);
			REQUIRE( cpu.GetRegisterAF().hi == 0x2f );
			check_flags( cpu, false, true, true, false );
		}
	}

	SECTION("AND r")
	{
		SECTION("AND TEST")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_andR(cpu, 0xff, 0x22);
			REQUIRE( cpu.GetRegisterAF().hi == 0x22 );

			call_andR(cpu, 0x33, 0xff);
			REQUIRE( cpu.GetRegisterAF().hi == 0x33 );

			call_andR(cpu, 0xff, 0x00);
			REQUIRE( cpu.GetRegisterAF().hi == 0x00 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}
	}

	SECTION("AND Imm8")
	{
		SECTION("AND TEST")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_andN(cpu, 0x80, 0xf0);
			REQUIRE( cpu.GetRegisterAF().hi == 0x80 );
			REQUIRE( cpu.GetFlagZ() == 0 );

			call_andN(cpu, 0x22, 0x11);
			REQUIRE( cpu.GetRegisterAF().hi == 0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}
	}


	SECTION("AND (HL)")
	{
		SECTION("AND TEST")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_andHL(cpu, 0x2f, 0x3030, 0xf2);
			REQUIRE( cpu.GetRegisterAF().hi == 0x22 );
			REQUIRE( cpu.GetFlagZ() == 0 );

			call_andHL(cpu, 0x3f, 0x4040, 0x00);
			REQUIRE( cpu.GetRegisterAF().hi == 0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}
	}

	SECTION("OR r")
	{
		SECTION("OR TEST")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_orR(cpu, 0x3f, 0x21);
			REQUIRE( cpu.GetRegisterAF().hi == 0x3f );
			NoFlagCheck( cpu );

			call_orR(cpu, 0x21, 0x24);
			REQUIRE( cpu.GetRegisterAF().hi == 0x25 );
			NoFlagCheck( cpu );

			call_orR(cpu, 0, 0);
			REQUIRE( cpu.GetRegisterAF().hi == 0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}
	}

	SECTION("OR n")
	{
		SECTION("OR TEST")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_orN(cpu, 0x56, 0x72);
			REQUIRE( cpu.GetRegisterAF().hi == 0x76 );
			NoFlagCheck( cpu );

			call_orN(cpu, 0x52, 0x59);
			REQUIRE( cpu.GetRegisterAF().hi == 0x5B );
			NoFlagCheck( cpu );

			call_orN(cpu, 0, 0);
			REQUIRE( cpu.GetRegisterAF().hi == 0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}
	}

	SECTION("OR (HL)")
	{
		SECTION("OR TEST")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_orHL(cpu, 0x56, 0x30f0, 0x72);
			REQUIRE( cpu.GetRegisterAF().hi == 0x76 );
			NoFlagCheck( cpu );

			call_orHL(cpu, 0x52, 0x3f21, 0x59);
			REQUIRE( cpu.GetRegisterAF().hi == 0x5B );
			NoFlagCheck( cpu );

			call_orHL(cpu, 0, 0x241f, 0);
			REQUIRE( cpu.GetRegisterAF().hi == 0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}
	}

	SECTION("XOR r")
	{
		SECTION("XOR TEST")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_xorR(cpu, 0x31, 0x93);
			REQUIRE( cpu.GetRegisterAF().hi == 0xA2 );
			NoFlagCheck( cpu );

			call_xorR(cpu, 0x52, 0x59);
			REQUIRE( cpu.GetRegisterAF().hi == 0xB );
			NoFlagCheck( cpu );

			call_xorR(cpu, 0, 0);
			REQUIRE( cpu.GetRegisterAF().hi == 0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}
	}

	SECTION("XOR n")
	{
		SECTION("XOR TEST")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_xorN(cpu, 0x21, 0x9);
			REQUIRE( cpu.GetRegisterAF().hi == 0x28 );
			NoFlagCheck( cpu );

			call_xorN(cpu, 0x42, 0x93);
			REQUIRE( cpu.GetRegisterAF().hi == 0xD1 );
			NoFlagCheck( cpu );

			call_xorN(cpu, 0, 0);
			REQUIRE( cpu.GetRegisterAF().hi == 0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}
	}

	SECTION("XOR HL")
	{
		SECTION("XOR TEST")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_xorHL(cpu, 0x82, 0xf2f2, 0x51);
			REQUIRE( cpu.GetRegisterAF().hi == 0xD3 );
			NoFlagCheck( cpu );

			call_xorHL(cpu, 0x49, 0xa2a2, 0x90);
			REQUIRE( cpu.GetRegisterAF().hi == 0xD9 );
			NoFlagCheck( cpu );

			call_xorHL(cpu, 0, 0x8080, 0);
			REQUIRE( cpu.GetRegisterAF().hi == 0 );
			REQUIRE( cpu.GetFlagZ() == 1 );
		}
	}

	SECTION("CP")
	{
		SECTION("N")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_cpN(cpu, 0x84, 0x32);
			REQUIRE( cpu.GetRegisterAF().hi == 0x84 );
			check_flags( cpu, false, false, true, false );

			call_cpN(cpu, 0x93, 0x86);
			REQUIRE( cpu.GetRegisterAF().hi == 0x93 );
			check_flags( cpu, false, true, true, false ); // H = set.

			call_cpN(cpu, 0x82, 0x91);
			REQUIRE( cpu.GetRegisterAF().hi == 0x82 );
			check_flags( cpu, false, false,true, true ); // C = Set.

			call_cpN(cpu, 0x82, 0x82);
			REQUIRE( cpu.GetRegisterAF().hi == 0x82 );
			check_flags( cpu, true, false, true, false );

			call_cpN(cpu, 0xFE, 0xFF);
			REQUIRE( cpu.GetRegisterAF().hi == 0xFE );
			check_flags( cpu, false, true, true, true );
		}

		SECTION("R")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_cpR(cpu, 0x84, 0x32);
			REQUIRE( cpu.GetRegisterAF().hi == 0x84 );
			check_flags( cpu, false, false, true, false );

			call_cpR(cpu, 0x93, 0x86);
			REQUIRE( cpu.GetRegisterAF().hi == 0x93 );
			check_flags( cpu, false, true, true, false ); // H = set.

			call_cpR(cpu, 0x82, 0x91);
			REQUIRE( cpu.GetRegisterAF().hi == 0x82 );
			check_flags( cpu, false, false,true, true ); // C = Set.

			call_cpR(cpu, 0x82, 0x82);
			REQUIRE( cpu.GetRegisterAF().hi == 0x82 );
			check_flags( cpu, true, false, true, false );

			call_cpR(cpu, 0xFE, 0xFF);
			REQUIRE( cpu.GetRegisterAF().hi == 0xFE );
			check_flags( cpu, false, true, true, true );
		}

		SECTION("HL")
		{
			cpu.Reset();
			NoFlagCheck( cpu );

			call_cpHL(cpu, 0x84, 0xf3f3, 0x32);
			REQUIRE( cpu.GetRegisterAF().hi == 0x84 );
			check_flags( cpu, false, false, true, false );

			call_cpHL(cpu, 0x93, 0x4920, 0x86);
			REQUIRE( cpu.GetRegisterAF().hi == 0x93 );
			check_flags( cpu, false, true, true, false ); // H = set.

			call_cpHL(cpu, 0x82, 0x3920, 0x91);
			REQUIRE( cpu.GetRegisterAF().hi == 0x82 );
			check_flags( cpu, false, false,true, true ); // C = Set.

			call_cpHL(cpu, 0x82, 0x3921, 0x82);
			REQUIRE( cpu.GetRegisterAF().hi == 0x82 );
			check_flags( cpu, true, false, true, false );

			call_cpHL(cpu, 0xFE, 0x3940, 0xFF);
			REQUIRE( cpu.GetRegisterAF().hi == 0xFE );
			check_flags( cpu, false, true, true, true );
		}
	}

	SECTION("INC")
	{
		SECTION("REGISTER")
		{
			cpu.Reset();

			incR( cpu, 0, 0x3 );
			REQUIRE( cpu.GetRegisterBC().hi == 0x4 );
			check_flags( cpu, false, false, false, false );

			incR( cpu, 1, 0x0f );
			REQUIRE( cpu.GetRegisterBC().lo == 0x10 );
			check_flags( cpu, false, true, false, false );

			incR( cpu, 2, 0xff );
			REQUIRE( cpu.GetRegisterDE().hi == 0x0 );
			check_flags( cpu, true, true, false, true );
		}

		SECTION("MEM HL")
		{
			cpu.Reset();

			incHL( cpu, 0x3000, 0x3 );
			REQUIRE( cpu.GetMemoryValue( 0x3000 ) == 0x4 );
			check_flags( cpu, false, false, false, false );

			incHL( cpu, 0x3040, 0x0f );
			REQUIRE( cpu.GetMemoryValue( 0x3040 ) == 0x10 );
			check_flags( cpu, false, true, false, false );

			incHL( cpu, 0x3422, 0xff );
			REQUIRE( cpu.GetMemoryValue( 0x3422 ) == 0x0 );
			check_flags( cpu, true, true, false, true );
		}
	}

	SECTION("DEC")
	{
		SECTION("REGISTER")
		{
			cpu.Reset();

			decR( cpu, 0, 0x3 );
			REQUIRE( cpu.GetRegisterBC().hi == 0x2 );
			check_flags( cpu, false, false, true, false );

			decR( cpu, 1, 0x10 );
			REQUIRE( cpu.GetRegisterBC().lo == 0x0f );
			check_flags( cpu, false, true, true, false );

			decR( cpu, 2, 0x00 );
			REQUIRE( cpu.GetRegisterDE().hi == 0xff );
			check_flags( cpu, false, true, true, true );
		}

		SECTION("HL")
		{
			cpu.Reset();

			decHL( cpu, 0x3000, 0x3 );
			REQUIRE( cpu.GetMemoryValue( 0x3000 ) == 0x2 );
			check_flags( cpu, false, false, true, false );

			decHL( cpu, 0x3001, 0x10 );
			REQUIRE( cpu.GetMemoryValue( 0x3001 ) == 0x0f );
			check_flags( cpu, false, true, true, false );

			decHL( cpu, 0x3002, 0x00 );
			REQUIRE( cpu.GetMemoryValue( 0x3002 ) == 0xff );
			check_flags( cpu, false, true, true, true );
		}
	}

	SECTION( "ADD HL, ss" )
	{
		// 0b00rr1001 { r = m16BitArguments }
		SECTION("ss = BC")
		{
			addHLFromReg16( cpu, 0x3000, 0x4000, 0b00 );
			REQUIRE( cpu.GetRegisterHL().reg_16 == 0x7000 );
			check_flags( cpu, false, false, false, false );

			///Half?
			addHLFromReg16( cpu, 0x0fff, 0x0001, 0b00);
			REQUIRE( cpu.GetRegisterHL().reg_16 == 0x1000 );
			check_flags( cpu, false, true, false, false );

			//Carry?
			addHLFromReg16( cpu, 0xf000, 0x1000, 0b00);
			REQUIRE( cpu.GetRegisterHL().reg_16 == 0x0000 );
			check_flags( cpu, false, false, false, true );

			// Carry and Half?
			addHLFromReg16( cpu, 0xffff, 0xffff, 0b00 );
			check_flags( cpu, false, true, false, true );
		}
	}

	SECTION( "INC ss" )
	{
		SECTION("ss = BC")
		{
			incReg16( cpu, 0x0000, 0b00 );
			REQUIRE( cpu.GetRegisterBC().reg_16 == 1 );
			check_flags( cpu, false, false, false, false );

			incReg16( cpu, 0x0fff, 0b00 );
			REQUIRE( cpu.GetRegisterBC().reg_16 == 0x1000 );
			check_flags( cpu, false, false, false, false );

			incReg16( cpu, 0xffff, 0b00 );
			REQUIRE( cpu.GetRegisterBC().reg_16 == 0x0 );
			check_flags( cpu, false, false, false, false );
		}
	}


	SECTION( "DEC ss" )
	{
		SECTION("ss = BC")
		{
			decReg16( cpu, 0x0000, 0b00 );
			REQUIRE( cpu.GetRegisterBC().reg_16 == 0xffff );
			check_flags( cpu, false, false, false, false );

			decReg16( cpu, 0x1000, 0b00 );
			REQUIRE( cpu.GetRegisterBC().reg_16 == 0x0fff );
			check_flags( cpu, false, false, false, false );

			decReg16( cpu, 0x00ff, 0b00 );
			REQUIRE( cpu.GetRegisterBC().reg_16 == 0x00fe );
			check_flags( cpu, false, false, false, false );
		}
	}

}