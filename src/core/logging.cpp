#include "logging.hpp"

#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

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

static std::string timestampNow() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << '.'
        << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

void ConsoleSink::write(LogLevel level, const std::string& message) {
	std::lock_guard<std::mutex> lock(g_mutex);
	std::ostream& out = level == LogLevel::Error ? std::cerr : std::cout;
	out << '[' << timestampNow() << "] [" << toCString(level) << "] " << message << '\n';
	out.flush();
}

FileSink::FileSink(const std::string& filePath) : path_(filePath) {
	// Write a session separator on creation
	std::lock_guard<std::mutex> lock(g_mutex);
	std::ofstream file(path_, std::ios::app);
	if (file) {
		file << "\n============ " << timestampNow() << " - session start ============\n";
		file.flush();
	}
}

void FileSink::write(LogLevel level, const std::string& message) {
	std::lock_guard<std::mutex> lock(g_mutex);
	std::ofstream file(path_, std::ios::app);
	if (!file) return;
	file << '[' << timestampNow() << "] [" << toCString(level) << "] " << message << '\n';
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


