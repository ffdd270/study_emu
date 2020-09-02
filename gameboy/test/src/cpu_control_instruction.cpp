//
// Created by nhy20 on 2020-09-02.
//

#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

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

}