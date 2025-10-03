#include "logging.hpp"

#include <iostream>
#include <fstream>
#include <mutex>

namespace core {

namespace {
	std::mutex g_mutex;
}

static LogLevel g_initialLevel = LogLevel::Info;

static const char* toCString(LogLevel level) {
	switch (level) {
		case LogLevel::Debug: return "DEBUG";
		case LogLevel::Info:  return "INFO";
		case LogLevel::Warn:  return "WARN";
		case LogLevel::Error: return "ERROR";
	}
	return "?";
}

void ConsoleSink::write(LogLevel level, const std::string& message) {
	std::lock_guard<std::mutex> lock(g_mutex);
	std::ostream& out = level == LogLevel::Error ? std::cerr : std::cout;
	out << '[' << toCString(level) << "] " << message << '\n';
}

FileSink::FileSink(const std::string& filePath) : path_(filePath) {}

void FileSink::write(LogLevel level, const std::string& message) {
	std::lock_guard<std::mutex> lock(g_mutex);
	std::ofstream file(path_, std::ios::app);
	if (!file) return;
	file << '[' << toCString(level) << "] " << message << '\n';
}

Logger& Logger::instance() {
	static Logger logger;
	return logger;
}

Logger::Logger() : currentLevel_(g_initialLevel) {
	// default console sink
	addSink(std::make_shared<ConsoleSink>());
}

void Logger::setLevel(LogLevel level) { currentLevel_ = level; }
LogLevel Logger::level() const { return currentLevel_; }

void Logger::addSink(std::shared_ptr<LogSink> sink) {
	sinks_.push_back(std::move(sink));
}

void Logger::log(LogLevel level, const std::string& message) {
	if (static_cast<int>(level) < static_cast<int>(currentLevel_)) return;
	for (auto& s : sinks_) {
		s->write(level, message);
	}
}

} // namespace core


