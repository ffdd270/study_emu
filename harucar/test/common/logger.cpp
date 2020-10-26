//
// Created by nhy20 on 2020-10-26.
//
#include <catch.hpp>
#include <common/common_logger.h>

using namespace HaruCar::Common::Log;

SCENARIO("Logger, Log.", "[Logger]")
{
	GIVEN("A Logger")
	{
		Logger logger;

		REQUIRE( logger.GetSize() == 0 );

		WHEN("Use Stream!")
		{
			std::string_view test_str = "This Is Not Bug.";

			LOG(logger) << test_str;
			LOG(logger) << "AB" << "CD";

			REQUIRE( logger.GetSize() == 2 );

			REQUIRE_NOTHROW( logger.GetData( 0 ) );
			REQUIRE( logger.GetData( 0 ).log  == test_str );
			REQUIRE( logger.GetData( 0 ).info == LogLevels::INFO );

			REQUIRE_NOTHROW( logger.GetData( 1 ) );
			REQUIRE( logger.GetData( 1 ).log == "ABCD" );
			REQUIRE( logger.GetData( 1 ).info == LogLevels::INFO );
		}

		WHEN( "Out of index" )
		{
			logger << "AB";
			logger << "CD";

			LOG(logger) << "AB" << "CD";
			REQUIRE_NOTHROW( logger.GetData( 0 ) );
			REQUIRE_THROWS( logger.GetData( 1 ) );
		}
	}
}