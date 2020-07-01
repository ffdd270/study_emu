//
// Created by nhy20 on 2020-06-18.
//



#include <catch.hpp>
#include "GameboyCPU.h"

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
			cpu.InjectionMemory( 0xEA ); // imm16.hi = 0xEA;
			cpu.InjectionMemory( 0x00 ); // imm16.lo = 0x00;
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
	
	SECTION( "LD r16, imm16" )
	{
		SECTION( "HL" )
		{
			cpu.Reset();

			cpu.InjectionMemory( 0b00100001 ); // LD HL (  01 ) imm 16 // 1 step.
			cpu.InjectionMemory( 0xDE ); // hi
			cpu.InjectionMemory( 0xAD ); // lo

			cpu.NextStep();

			REQUIRE(cpu.GetRegisterHL().reg_16 == 0xDEAD );
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

		cpu.InjectionMemory( 0b00100001 ); // LD HL(01), imm16 // 1 step.
		cpu.InjectionMemory( 0xEA ); // imm_hi = 0xEA
		cpu.InjectionMemory( 0x00 ); // imm_low = 0x00


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



}





//InjectionMemory