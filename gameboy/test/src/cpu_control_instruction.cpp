//
// Created by nhy20 on 2020-09-02.
//

#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

inline void setRegAndStep( GameboyCPU & cpu, BYTE byte )
{
	setRegister8( cpu,  0b111, byte );
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
			subHL( cpu, 0x99, 0x3000, 0xA0 );
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
			setRegAndStep( cpu, origin_value);
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
			subHL( cpu, 0x99, 0x3000, 0xA0 ); // Carry.
			REQUIRE( cpu.GetFlagC() );

			ccf( cpu );
			REQUIRE( cpu.GetFlagC() == false );
			REQUIRE( cpu.GetFlagN() == false );
			REQUIRE( cpu.GetFlagH() == true ); // copy of previous carry.
		}
	}
}