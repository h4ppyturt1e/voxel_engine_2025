#include "logging.hpp"

#include <iostream>

namespace core {

static LogLevel currentLevel = LogLevel::Info;

void setLogLevel(LogLevel level) {
	currentLevel = level;
}

static const char* toCString(LogLevel level) {
	switch (level) {
		case LogLevel::Debug: return "DEBUG";
		case LogLevel::Info:  return "INFO";
		case LogLevel::Warn:  return "WARN";
		case LogLevel::Error: return "ERROR";
	}
	return "?";
}

void log(LogLevel level, const std::string& message) {
	if (static_cast<int>(level) < static_cast<int>(currentLevel)) {
		return;
	}
	std::ostream& out = level == LogLevel::Error ? std::cerr : std::cout;
	out << '[' << toCString(level) << "] " << message << '\n';
}

} // namespace core


