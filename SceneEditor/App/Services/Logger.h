#pragma once

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;


enum class LogLevel : unsigned int
{
	DEBUG,
	INFO,
	WARNING,
	ERROR
};


class Logger
{
public:

	static void LogDebug(string channel, string message);
	static void LogInfo(string channel, string message);
	static void LogWarning(string channel, string message);
	static void LogError(string channel, string message);

private:

	static void Log(string channel, string message, string level);

	static const LogLevel MIN_LOG_LEVEL = LogLevel::DEBUG;
};
