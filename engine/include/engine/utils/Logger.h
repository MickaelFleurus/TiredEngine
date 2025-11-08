#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <source_location>
#include <string>
#include <string_view>
#include <thread>

namespace Utils {
enum class LogLevel { Trace, Info, Warning, Error, Fatal };

class Logger {
public:
    static void Init(std::string filename);
    static void Shutdown();

#ifndef NDEBUG
    static constexpr LogLevel MinLevel = LogLevel::Trace;
#else
    static constexpr LogLevel MinLevel = LogLevel::Warning;
#endif

    template <LogLevel Level, typename... Args>
    static void Log(const std::source_location& loc, std::string_view fmt,
                    Args&&... args) {
        if constexpr (Level >= MinLevel) {
            WriteAsync(Level, std::vformat(fmt, std::make_format_args(args...)),
                       loc);
        }
    }
    static void Flush();

private:
    struct LogMessage {
        LogLevel level;
        std::string text;
        std::string file;
        uint32_t line;
        std::chrono::system_clock::time_point timestamp;
    };

    static void WriteAsync(LogLevel level, const std::string& msg,
                           const std::source_location& loc);
    static void Worker();
    static void Enqueue(LogMessage&& message);
    static void FlushMessage(const LogMessage& msg);

    static inline std::thread s_Thread;
    static inline std::mutex s_Mutex;
    static inline std::condition_variable s_CondVar;
    static inline std::queue<LogMessage> s_Queue;
    static inline std::ofstream s_File;
    static inline std::atomic<bool> s_Running{false};
};
} // namespace Utils

// Convenient macros for compile-time log level filtering
#define LOG_TRACE(fmt, ...)                                                    \
    Utils::Logger::Log<Utils::LogLevel::Trace>(                                \
        std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)                                                     \
    Utils::Logger::Log<Utils::LogLevel::Info>(std::source_location::current(), \
                                              fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...)                                                  \
    Utils::Logger::Log<Utils::LogLevel::Warning>(                              \
        std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)                                                    \
    Utils::Logger::Log<Utils::LogLevel::Error>(                                \
        std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...)                                                    \
    Utils::Logger::Log<Utils::LogLevel::Fatal>(                                \
        std::source_location::current(), fmt, ##__VA_ARGS__)
