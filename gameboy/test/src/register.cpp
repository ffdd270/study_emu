//
// Created by nhy20 on 2020-06-14.
//

#include <catch.hpp>
#include "GameboyCPU.h"

union Test
{
	int arr[4];

	struct
	{
		int _0, _1, _2, _3;
	};

};


TEST_CASE( "Register Test Code", "[REG]" )
{
	Register reg;

	reg.reg_16 = 0xddff;

	REQUIRE( reg.hi == 0xdd );
	REQUIRE( reg.lo == 0xff );
	REQUIRE( reg.reg_16 == 0xddff );

	Test test = { 0, 1, 2, 3 };

	REQUIRE(test._0 == 0);
	REQUIRE(test._1 == 1);
	REQUIRE(test._2 == 2);
	REQUIRE(test._3 == 3);
}