#include "Logger.h"


void Logger::LogDebug(string channel, string message)
{
	if (MIN_LOG_LEVEL <= LogLevel::DEBUG)
	{
		Log("DEBUG", channel, message);
	}
}

void Logger::LogInfo(string channel, string message)
{
	if (MIN_LOG_LEVEL <= LogLevel::INFO)
	{
		Log("INFO", channel, message);
	}
}

void Logger::LogWarning(string channel, string message)
{
	if (MIN_LOG_LEVEL <= LogLevel::WARNING)
	{
		Log("WARNING", channel, message);
	}
}

void Logger::LogError(string channel, string message)
{
	if (MIN_LOG_LEVEL <= LogLevel::ERROR)
	{
		Log("ERROR", channel, message);
	}
}

void Logger::Log(string level, string channel, string message)
{
	string logMessage = level + "::" + channel + ": " + message;
	cout << logMessage << endl;
}
