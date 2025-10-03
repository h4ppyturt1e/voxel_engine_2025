#pragma once

#include <string>

namespace core {

enum class LogLevel {
	Debug,
	Info,
	Warn,
	Error
};

void setLogLevel(LogLevel level);
void log(LogLevel level, const std::string& message);

} // namespace core


