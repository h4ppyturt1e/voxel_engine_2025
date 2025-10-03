#pragma once

#include <string>
#include <memory>
#include <vector>

namespace core {

enum class LogLevel {
	Debug,
	Info,
	Warn,
	Error
};

class LogSink {
public:
	virtual ~LogSink() = default;
	virtual void write(LogLevel level, const std::string& message) = 0;
};

class ConsoleSink : public LogSink {
public:
	void write(LogLevel level, const std::string& message) override;
};

class FileSink : public LogSink {
public:
	explicit FileSink(const std::string& filePath);
	void write(LogLevel level, const std::string& message) override;

private:
	std::string path_;
};

class Logger {
public:
	static Logger& instance();

	void setLevel(LogLevel level);
	LogLevel level() const;
	void addSink(std::shared_ptr<LogSink> sink);
	void log(LogLevel level, const std::string& message);

private:
	Logger();
	LogLevel currentLevel_;
	std::vector<std::shared_ptr<LogSink>> sinks_;
};

// Convenience wrappers
inline void setLogLevel(LogLevel level) { Logger::instance().setLevel(level); }
inline void log(LogLevel level, const std::string& message) { Logger::instance().log(level, message); }

// Macros to include source file and line
#define VOX_LOG_DEBUG(msg) ::core::Logger::instance().log(::core::LogLevel::Debug, std::string("[") + __FILE__ + ":" + std::to_string(__LINE__) + "] " + (msg))
#define VOX_LOG_INFO(msg)  ::core::Logger::instance().log(::core::LogLevel::Info,  (msg))
#define VOX_LOG_WARN(msg)  ::core::Logger::instance().log(::core::LogLevel::Warn,  (msg))
#define VOX_LOG_ERROR(msg) ::core::Logger::instance().log(::core::LogLevel::Error, (msg))

} // namespace core


