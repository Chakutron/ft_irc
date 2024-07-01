#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>

class Logger
{
public:
	enum LogLevel {
		DEBUG,
		INFO,
		WARNING,
		ERROR
	};

	static void init(const std::string& filename);
	static void log(LogLevel level, const std::string& message);

private:
	static std::ofstream logFile;
	static LogLevel currentLevel;

	static std::string levelToString(LogLevel level);
	static const char* getColorCode(LogLevel level);
};

#define LOG_DEBUG(message) Logger::log(Logger::DEBUG, message)
#define LOG_INFO(message) Logger::log(Logger::INFO, message)
#define LOG_WARNING(message) Logger::log(Logger::WARNING, message)
#define LOG_ERROR(message) Logger::log(Logger::ERROR, message)

#endif // LOGGER_HPP