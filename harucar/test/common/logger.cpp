//
// Created by nhy20 on 2020-10-26.
//
#include <catch.hpp>
#include <common/common_logger.h>

using namespace HaruCar::Common::Log;

inline void check_for( std::vector<LogData> & ref_last_logs, size_t begin, size_t end, size_t add_num )
{
	for( size_t i = begin; i < end; i++ )
	{
		auto & ref_log = ref_last_logs[i];

		REQUIRE( ref_log.info == LogLevels::INFO );
		REQUIRE( ref_log.log == "log " + std::to_string( i + add_num ) );
	}


}

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

			THEN("Size 2, Same as Input String.")
			{
				REQUIRE( logger.GetSize() == 2 );

				REQUIRE_NOTHROW( logger.GetData( 0 ) );
				REQUIRE( logger.GetData( 0 ).log  == test_str );
				REQUIRE( logger.GetData( 0 ).info == LogLevels::INFO );

				REQUIRE_NOTHROW( logger.GetData( 1 ) );
				REQUIRE( logger.GetData( 1 ).log == "ABCD" );
				REQUIRE( logger.GetData( 1 ).info == LogLevels::INFO );
			}

			THEN("Out of Index, require throw.")
			{
				REQUIRE_THROWS( logger.GetData( 3 ) );
			}
		}
	}

	GIVEN("A Logger with three log.")
	{
		Logger logger;
		logger.LogInfo("log 1");
		logger.LogInfo("log 2");
		logger.LogInfo("log 3");

		THEN("last get 3.")
		{
			REQUIRE( logger.GetLogSizeFromLastGet() == 3 );
		}

		WHEN("Get last, and add two.")
		{
			auto last_logs = logger.GetLogsFromLastGet();
			REQUIRE( last_logs.size() == 3 );

			check_for( last_logs, 0, 3, 1 );

			logger.LogInfo("log 4");
			logger.LogInfo("log 5");

			THEN("last indices two.")
			{
				REQUIRE(logger.GetLogSizeFromLastGet() == 2);
				auto last_logs_two = logger.GetLogsFromLastGet();

				REQUIRE( last_logs_two.size() == 2 );

				check_for( last_logs_two, 0, 2, 4 );
			}
		}
	}
}