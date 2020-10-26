//
// Created by nhy20 on 2020-10-26.
//

#include <catch.hpp>
#include "util/util_string.h"

TEST_CASE( "UTIL STRING", "[UTIL]" )
{
	SECTION("String Split")
	{
		SECTION("Basic")
		{
			std::string_view test = "0x24424\n0x23242";

			auto split_result = HaruCar::Util::string_split(test);

			REQUIRE(split_result.size() == 2);
			REQUIRE(split_result[0] == "0x24424");
			REQUIRE(split_result[1] == "0x23242");
		}


		SECTION("NO SPLIT")
		{
			std::string_view test = "0x24242424";

			std::vector<std::string> split_result = HaruCar::Util::string_split( test );

			REQUIRE( split_result.size() == 1 );
			REQUIRE( split_result[0] == "0x24242424" );
		}
	}


}