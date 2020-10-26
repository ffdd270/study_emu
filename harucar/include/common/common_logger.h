//
// Created by HaruGakkaP on 2020-10-26.
//

#ifndef HARUCAR_COMMON_LOGGER_H
#define HARUCAR_COMMON_LOGGER_H

#include <string>
#include <string_view>

#include <vector>

namespace HaruCar::Common::System
{
	enum class LogLevels
	{
		ERROR,
		WARNING,
		INFO
	};

	struct LogData
	{
		std::string log;
		LogLevels info;
	};


	class Logger
	{
	public:
		void Log( LogLevels level, std::string_view str );

		void LogWarn( std::string_view str ) { Log(  LogLevels::WARNING, str ); }
		void LogError( std::string_view str ) { Log(  LogLevels::ERROR, str ); }
		void LogInfo( std::string_view str ) { Log(  LogLevels::INFO, str ); }
	private:
		std::vector<LogData> mLogData;
	};
}

#endif //HARUCAR_COMMON_LOGGER_H
