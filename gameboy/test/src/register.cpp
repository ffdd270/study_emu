//
// Created by nhy20 on 2020-06-14.
//

#include <catch.hpp>
#include "GameboyCPU.h"

TEST_CASE( "Register Test Code", "[REG]" )
{
	Register reg;

	reg.reg_16 = 0xddff;

	REQUIRE( reg.hi == 0xdd );
	REQUIRE( reg.lo == 0xff );
	REQUIRE( reg.reg_16 == 0xddff );
}

