#include "../inc/Logger.hpp"
#include <iostream>
#include <ctime>
#include <cstring>

std::ofstream Logger::logFile;
Logger::LogLevel Logger::currentLevel = Logger::INFO;

void Logger::init(const std::string& filename)
{
	logFile.open(filename.c_str(), std::ios::app);
	if (!logFile.is_open())
	{
		std::cerr << "Failed to open log file: " << filename << std::endl;
	}
}

void Logger::log(LogLevel level, const std::string& message)
{
	if (level >= currentLevel)
	{
		time_t now = time(0);
		char* dt = ctime(&now);
		dt[strlen(dt) - 1] = '\0'; // Remove newline

		std::string colorCode = getColorCode(level);
		std::string logMessage = std::string(dt) + " [" + levelToString(level) + "] " + message + "\n";
		
		std::cout << colorCode << logMessage << RESET << std::endl;
		if (logFile.is_open())
		{
			logFile << logMessage;
			logFile.flush();
		}
	}
}

std::string Logger::levelToString(LogLevel level)
{
	switch (level)
	{
		case DEBUG: return "DEBUG";
		case INFO: return "INFO";
		case WARNING: return "WARNING";
		case ERROR: return "ERROR";
		default: return "UNKNOWN";
	}
}

const char* Logger::getColorCode(LogLevel level)
{
    switch (level)
    {
        case DEBUG: return BLUE;
        case INFO: return GREEN;
        case WARNING: return YELLOW;
        case ERROR: return RED;
        default: return "";
    }
}