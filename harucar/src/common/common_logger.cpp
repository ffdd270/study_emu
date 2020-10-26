//
// Created by HaruGakkaP on 2020-10-26.
//

#include "common/common_logger.h"
#include <stdexcept>

using namespace HaruCar;
using namespace HaruCar::Common::Log;


void Logger::Log(LogLevels level, std::string_view str)
{
	mLogData.emplace_back( LogData{ .log =  str.data(), .info = level } );
}

Logger & Logger::operator<<(const std::string_view &str)
{
	this->mStream += str;
	return *this;
}

const LogData &Logger::GetData(size_t index) const
{
	if ( mLogData.size() <= index ) {  throw std::out_of_range("Expect mLogData.size() <= index."); }
	return mLogData[ index ];
}

const std::vector<LogData> &Logger::GetDatas() const
{
	return mLogData;
}

void Logger::logStreamEnd()
{
	LogInfo( mStream );
	mStream.clear();
}

void LoggerEnd::operator=(Logger &other)
{
	other.logStreamEnd();
}
