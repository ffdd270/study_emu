//
// Created by nhy20 on 2020-09-02.
//

#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

inline void setRegAAndStep(GameboyCPU & cpu, BYTE byte )
{
	setRegister8(cpu, Param8BitIndex::A, byte );
	cpu.NextStep();
}

TEST_CASE( "CPU CONTROL INSTRUCTION", "[CPU]" )
{
	GameboyCPU cpu;

	SECTION("DAA Test")
	{
		SECTION("Add A to HL as DAA ")
		{
			addAtoHL( cpu,  0x3434, 0x21, 0x39 );
			daa( cpu );

			REQUIRE( cpu.GetRegisterAF().hi == 0x60 );
		}

		SECTION("SUB A to HL as DAA")
		{
			call_subHL(cpu, 0x99, 0x3000, 0xA0);
			daa( cpu );

			REQUIRE( cpu.GetFlagC() == true );
			REQUIRE( cpu.GetRegisterAF().hi == 0x99 );
		}

		SECTION("ADD A to HL as DAA and Overflow")
		{
			addAtoHL( cpu,  0x3434, 0x50, 0x51 );
			daa( cpu );

			REQUIRE( cpu.GetFlagC() == true );
			REQUIRE( cpu.GetRegisterAF().hi == 0x1 );
		}
	}


	SECTION("CPL Test")
	{
		SECTION("NOT A?")
		{
			BYTE origin_value = 0b010101010;
			setRegAAndStep(cpu, origin_value);
			cpl( cpu );

			REQUIRE( cpu.GetRegisterAF().hi == static_cast<BYTE>(~origin_value) );
			REQUIRE( cpu.GetFlagH() );
			REQUIRE( cpu.GetFlagN() );
		}
	}

	SECTION("CCF Test")
	{
		SECTION("Set Flag, and CCF, and Check Carry Flag")
		{
			call_subHL(cpu, 0x99, 0x3000, 0xA0); // Carry.
			REQUIRE( cpu.GetFlagC() );

			ccf( cpu );
			REQUIRE( cpu.GetFlagC() == false );
			REQUIRE( cpu.GetFlagN() == false );
			REQUIRE( cpu.GetFlagH() == true ); // copy of previous carry.
		}
	}

	SECTION("SCF Test")
	{
		SECTION("Set Flag.")
		{
			cpu.Reset();

			REQUIRE( cpu.GetFlagC() == false );
			scf( cpu );
			REQUIRE( cpu.GetFlagC() == true );
		}
	}

	SECTION("NOP Test") // 테스트가 생각나면 다시..
	{
		cpu.Reset();

		cpu.InjectionMemory( 0x00 );
		cpu.NextStep();
	}

	SECTION("HALT Test")
	{
		setRegAAndStep(cpu, 0x30);
		REQUIRE( cpu.GetRegisterAF().hi == 0x30 );

		cpu.InjectionMemory( 0x76 );
		cpu.NextStep();
		REQUIRE( cpu.IsHalted() );

		addAtoHL( cpu,  0x3434, 0x21, 0x42 ); // No Effect.
		REQUIRE( cpu.GetRegisterAF().hi == 0x30 );
	}


	 // TODO : 인터럽트 테스트는 나중에... 유무만 확인하자.
	SECTION("DI Test")
	{
		cpu.InjectionMemory( 0xF3 );
		cpu.NextStep();
		REQUIRE(cpu.IsInterruptEnable() == false );
	}

	SECTION("EI Test")
	{
		cpu.InjectionMemory( 0xF3 );
		cpu.NextStep();
		REQUIRE(cpu.IsInterruptEnable() == false );

		cpu.InjectionMemory( 0xFB );
		cpu.NextStep();
		REQUIRE(cpu.IsInterruptEnable() == true );
	}

}