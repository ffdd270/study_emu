//
// Created by nhy20 on 2020-06-18.
//



#include <catch.hpp>
#include "GameboyCPU.h"

TEST_CASE( "CPU Code", "[REG]" )
{
	GameboyCPU cpu;

	cpu.Reset();

	SECTION( "LD B, 0x3")
	{
		cpu.InjectionCode( 0x06 );
		cpu.InjectionCode( 0x03 );

		cpu.NextStep();

		auto reg = cpu.getRegisterBC();
		REQUIRE( reg.hi == 0x03 );
	}
}





//InjectionCode