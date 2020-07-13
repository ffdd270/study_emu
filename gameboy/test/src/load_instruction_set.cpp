//
// Created by nhy20 on 2020-06-18.
//



#include <catch.hpp>
#include "GameboyCPU.h"

inline void setRegister16( GameboyCPU & cpu_ref ,BYTE register_index, WORD value )
{
	BYTE hi = ( 0xFF00 & value ) >> 8; // hi byte,
	BYTE lo = 0x00FF & value; // lo byte

	cpu_ref.InjectionMemory( 0b00000001 | ( register_index << 4 ) );
	cpu_ref.InjectionMemory( lo );
	cpu_ref.InjectionMemory( hi );
}

inline void setRegister8( GameboyCPU & cpu_ref, BYTE register_index, BYTE value )
{
	cpu_ref.InjectionMemory( 0b00000110 | ( register_index << 3 ) ); // LD B, imm8
	cpu_ref.InjectionMemory( value ); // imm
}

// 3 clock.
inline void setMemory3Step(GameboyCPU & cpu_ref, BYTE reg8_index, WORD mem_addr, BYTE value )
{

	setRegister16(  cpu_ref, 0b10, mem_addr ); // LD HL(10), imm16( mem_addr )
	//HL = mem_addr. 1 Step.

	setRegister8( cpu_ref, reg8_index, value ); // LD E, 0xBA
	//reg8_index = value. 2 Step.

	cpu_ref.InjectionMemory( 0b01110000 | reg8_index ); // LD (HL), reg8_index
	//(HL) = value  3 Step.
}

TEST_CASE( "CPU Code", "[REG]" )
{
	GameboyCPU cpu;


	SECTION( "LD B, 0x3") // LD r, n
	{
		cpu.Reset();

		cpu.InjectionMemory(0x06); // LD B, imm
		cpu.InjectionMemory(0x03); // imm  =  0x3

		cpu.NextStep();

		auto reg = cpu.GetRegisterBC();
		REQUIRE( reg.hi == 0x03 );
	}


	SECTION( "LD R1, R2" ) // LD reg to reg
	{

		SECTION( "LD B, C" )
		{
			cpu.Reset();


			cpu.InjectionMemory(0x0E);  //LD C, imm
			cpu.InjectionMemory(0x0A);  //imm = 0xA
			// Step.

			cpu.InjectionMemory(0x41); // LD B, C.
			// Expect B = 0xA
			// Step.

			cpu.NextStep(); //LD C, imm
			cpu.NextStep(); //LD B, C.

			auto reg = cpu.GetRegisterBC();
			REQUIRE( reg.hi == 0xA );
		}



		SECTION( "LD C, B" )
		{
			cpu.Reset();


			cpu.InjectionMemory(0x06);  //LD B, imm
			cpu.InjectionMemory(0xAA);  //imm = 0xAA
			// Step.

			cpu.InjectionMemory(0x48); // LD C, B.
			// Expect C = 0xAA
			// Step.

			cpu.NextStep(); //LD B, imm
			cpu.NextStep(); //LD C, B.

			auto reg = cpu.GetRegisterBC();
			REQUIRE( reg.lo == 0xAA );
		}



		SECTION( "LD (HL), B" )
		{
			cpu.Reset();

			cpu.InjectionMemory(0x06);  //LD B, imm
			cpu.InjectionMemory(0xAA);  //imm = 0xAA
			// Step.

			cpu.InjectionMemory(0x21); // LD HL, imm16
			cpu.InjectionMemory( 0x00 ); // imm16.lo = 0x00;
			cpu.InjectionMemory( 0xEA ); // imm16.hi = 0xEA;
			// HL = 0xEA00;

			cpu.InjectionMemory(0x70); // LD (HL), B. (HL) = mGameMemory[ (HL) ]
			// Expect C = 0xAA
			// Step.

			cpu.NextStep(); //LD B, imm
			cpu.NextStep(); //LD HL, mm16
			cpu.NextStep(); //LD (HL), B == LD 0xEA00, B.

			BYTE mem_value = cpu.GetMemoryValue( 0xEA00 );
			REQUIRE( mem_value == 0xAA );
		}
	}


	SECTION( "LD r8, (HL)" )
	{
		cpu.Reset();

		/*
		 * A 111
		 * B 000
		 * C 001
		 * D 010
		 * E 011
		 * H 100
		 * L 101
		 */

		setRegister16(  cpu, 0b10, 0xEA00 ); // LD HL(10), imm16( 0xEA00 )  // 1 step.

		cpu.InjectionMemory( 0b00111110 ); //LD A, n // 2 step.
		cpu.InjectionMemory( 0xAA ); //n = 0xAA

		cpu.InjectionMemory( 0b01110111 ); //LD (HL), A (HL) = AA // 3 step.
		cpu.InjectionMemory( 0b01000110 ); //LD B, (HL) // 4 step.

		cpu.NextStep();
		cpu.NextStep();
		cpu.NextStep();
		cpu.NextStep();

		REQUIRE(cpu.GetRegisterBC().hi == 0xAA);
	}


	SECTION( "LD A, (BC)" )
	{
		cpu.Reset();

		setRegister16(  cpu, 0b10, 0xDEAD ); // LD HL(10), imm16( 0xDEAD )
		// HL = 0xDEAD. 1 Step.

		cpu.InjectionMemory( 0b00001110 ); // LD B, imm8
		cpu.InjectionMemory( 0xEA ); // imm = 0xEA
		// B = 0xEA. 2 Step.

		cpu.InjectionMemory( 0b01110001 ); // LD (HL), B
		// (0xDEAD) = 0xEA. 3 Step.

		setRegister16( cpu, 0, 0xDEAD );// LD BC( 0 ) , imm16 ( 0xDEAD )
		// BC = 0xDEAD. 4 Step.


		cpu.InjectionMemory( 0b00001010 ); // LD A, (BC)
		// A = (BC)  == A = (0xDEAD [ 0xEA ])
		// 5 Step.

		for( int i = 0; i < 5; i++ ) { cpu.NextStep(); }

		REQUIRE( cpu.GetRegisterAF().hi == 0xEA );
	}

	SECTION( "LD A, (DE)")
	{
		cpu.Reset();

		setRegister8( cpu, 0b11, 0xBA ); // LD E, 0xBA
		//E = 0xBA. 1 Step.

		setRegister16(  cpu, 0b10, 0xFAFA ); // LD HL(10), imm16( 0xFAFA )
		//HL = 0xFAFA. 2 Step.

		cpu.InjectionMemory( 0b01110011 ); // LD (HL), E
		//(0xFAFA) = 0xBA. 3 Step.

		setRegister16( cpu, 0b01, 0xFAFA ); // LD DE, imm16 ( 0xFAFA )
		//DE = 0xFAFA. 4  Step.

		cpu.InjectionMemory( 0b00011010 ); // LD A, ( DE )
		//A = (DE) == A = ( 0xFAFA ( 0xBA ) ) 5 Step.

		for( int i = 0; i < 5; i++ ) {  cpu.NextStep(); }

		REQUIRE( cpu.GetRegisterAF().hi == 0xBA );
	}

	/* Only on Z80
	SECTION( "LD A, (nn)" )
	{
		cpu.Reset();

		setMemory3Step(cpu, 0b11, 0xF00D, 0x42);
		// 3 Step.

		cpu.InjectionMemory( 0b00111010 ); // LD A, (nn)
		cpu.InjectionMemory( 0x0D ); // imm_lo
		cpu.InjectionMemory( 0xF0 ); // imm_hi

		// A = (0xF00D ( 0x42 )  ) 4 Step.

		for( int i = 0; i < 4; i++ ) { cpu.NextStep(); }

		REQUIRE( cpu.GetRegisterAF().hi == 0x42 );
	}*/


	// (DE)<-A
	SECTION( "LD (DE), A" )
	{
		cpu.Reset();

		setRegister16( cpu, 0b01, 0xDAFA ); // LD DE, 0xDAFA
		setRegister8( cpu, 0b111, 0xA0 ); // LD A, 0xA0
		cpu.InjectionMemory( 0b00010010 ); //LD (DE), A

		for( int i = 0 ; i < 3; i++ ) { cpu.NextStep(); }

		REQUIRE( cpu.GetMemoryValue( 0xDAFA ) == 0xA0 );
	}

	// (HL)<-A and HL<-HL + 1
	SECTION( "LDI (HL), A" )
	{
		cpu.Reset();

		setRegister16( cpu, 0b10, 0xAEAD ); // LD HL, 0xAEAD
		setRegister8(cpu, 0b111, 0xAD ); // LD A, 0xAD
		cpu.InjectionMemory( 0b00100010 ); //LDI (HL), A

		for ( int i = 0 ; i < 3; i++ ) { cpu.NextStep(); }

		REQUIRE( cpu.GetMemoryValue( 0xAEAD ) == 0xAD );
		REQUIRE( cpu.GetRegisterHL().reg_16 == 0xAEAE );

	}

	// (HL)<A and HL<-HL - 1
	SECTION( "LDD (HL), A" )
	{
		cpu.Reset();

		setRegister16( cpu, 0b10, 0xDED1 ); // LD HL, 0xDED1
		setRegister8( cpu, 0b111, 0xDA ); //LD A, 0xDA
		cpu.InjectionMemory( 0b00110010 ); //LDD (HL), A

		for( int i = 0; i < 3; i++ ) { cpu.NextStep(); }

		REQUIRE( cpu.GetMemoryValue( 0xDED1 ) == 0xDA );
		REQUIRE( cpu.GetRegisterHL().reg_16 == 0xDED0 );
	}

	// A<-(HL) and HL--;
	SECTION( "LDD A, (HL)" )
	{
		cpu.Reset();

		setMemory3Step( cpu, 0b00, 0xE001, 0x10 );  // HL = 0xE001, B = 0x10, (HL) = B.
		cpu.InjectionMemory( 0b00111010 ); // LDD A,(HL)

		for( int i = 0; i < 4; i++ ){ cpu.NextStep(); }

		REQUIRE( cpu.GetRegisterAF().hi == 0x10 );
		REQUIRE( cpu.GetRegisterHL().reg_16 == 0xE000 );
	}

	SECTION( "LDI A, (HL)" )
	{
		cpu.Reset();

		setMemory3Step( cpu, 0b00, 0x2FFF, 0x30 );  // HL = 0x2FFF, B = 0x30, (HL) = B.
		cpu.InjectionMemory( 0b00101010 ); // LDI A, (HL)

		for( int i = 0; i < 4; i++ ) { cpu.NextStep(); }

		REQUIRE( cpu.GetRegisterAF().hi == 0x30 );
		REQUIRE( cpu.GetRegisterHL().reg_16 == 0x3000 );
	}

	SECTION( "LD (BC), A" )
	{
		cpu.Reset();


		setRegister16( cpu, 0b00, 0xEAFA ); // LD BC, imm16 ( 0xEAFA  )
		// BC = 0xEAFA. 1 Step.

		setRegister8( cpu, 0b111, 0xCF ); // LD A, imm8 ( 0xCF )
		// A = 0xCF. 2 Step.

		cpu.InjectionMemory( 0b00000010 ); // LD ( BC ), A
		// ( BC ) = A == ( 0xEAFA ) = 0xCF. 3 Step.

		for ( int i = 0; i < 3; i++ ) {  cpu.NextStep(); }

		REQUIRE( cpu.GetMemoryValue( 0xEAFA ) == 0xCF );
	}


	SECTION("LD (DE), A")
	{
		cpu.Reset();
	}



	SECTION( "LD r16, imm16" )
	{
		SECTION( "HL" )
		{
			cpu.Reset();

			cpu.InjectionMemory( 0b00100001 ); // LD HL (  01 ) imm 16 // 1 step.
			cpu.InjectionMemory( 0xAD ); // lo
			cpu.InjectionMemory( 0xDE ); // hi

			cpu.NextStep();

			REQUIRE(cpu.GetRegisterHL().reg_16 == 0xDEAD );
		}
	}
}





//InjectionMemory