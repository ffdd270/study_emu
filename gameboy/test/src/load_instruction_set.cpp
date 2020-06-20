//
// Created by nhy20 on 2020-06-18.
//



#include <catch.hpp>
#include "GameboyCPU.h"

TEST_CASE( "CPU Code", "[REG]" )
{
	GameboyCPU cpu;


	SECTION( "LD B, 0x3")
	{
		cpu.Reset();

		cpu.InjectionMemory(0x06); // LD B, imm
		cpu.InjectionMemory(0x03); // imm  =  0x3

		cpu.NextStep();

		auto reg = cpu.GetRegisterBC();
		REQUIRE( reg.hi == 0x03 );
	}


	SECTION( "LD R1, R2" )
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
}





//InjectionMemory