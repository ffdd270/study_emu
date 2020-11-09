//
// Created by HaruGakkaP on 2020-10-26.
//

#ifndef HARUCAR_COMMON_LOGGER_H
#define HARUCAR_COMMON_LOGGER_H

#include <string>
#include <string_view>

#include <vector>

namespace HaruCar::Common::Log
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

	class LoggerEnd;

	class Logger
	{
	public:
		friend class LoggerEnd;
		void Log( LogLevels level, std::string_view str );

		void LogWarn( std::string_view str ) { Log(  LogLevels::WARNING, str ); }
		void LogError( std::string_view str ) { Log(  LogLevels::ERROR, str ); }
		void LogInfo( std::string_view str ) { Log(  LogLevels::INFO, str ); }

		Logger & operator<<( const std::string_view & str );

		// 마지막 획득으로부터 이후.
		[[nodiscard]] std::vector<LogData> GetLogsFromLastGet();
		[[nodiscard]] size_t GetLogSizeFromLastGet() const;

		// 마지막 획득 초기화
		void ResetLastGetIndex();

		[[nodiscard]] const LogData & GetData( size_t index ) const;
		[[nodiscard]] const std::vector<LogData> & GetDatas() const;
		[[nodiscard]] size_t GetSize() const { return mLogData.size(); }
	private:
		void logStreamEnd();

		size_t mLastGetIndex = 0;

		std::string mStream;
		std::vector<LogData> mLogData;
	};

	class LoggerEnd
	{
	public:
		void operator=(Logger & other);
	};

#define LOG( Logger )\
	HaruCar::Common::Log::LoggerEnd() = Logger
}

#endif //HARUCAR_COMMON_LOGGER_H
