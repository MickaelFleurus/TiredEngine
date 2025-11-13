#include "engine/utils/Logger.h"
#include <array>
#include <ctime>
#include <filesystem>

#include <vulkan/vulkan.h>

namespace {

std::string FormatTimeLog(std::chrono::system_clock::time_point tp) {
    auto time_t = std::chrono::system_clock::to_time_t(tp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  tp.time_since_epoch()) %
              1000;

    std::tm* tm = std::localtime(&time_t);

    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    return std::format("{}.{:03d}", std::string(buf), ms.count());
}

std::string FormatTimeFilename() {
    auto time_t =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::tm* tm = std::localtime(&time_t);

    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", tm);
    return std::format("/{}.txt", std::string(buf));
}

constexpr std::array<std::string_view, 5> kLevelStrs{"TRACE", "INFO ", "WARN ",
                                                     "ERROR", "FATAL"};

} // namespace

namespace Utils {
void Logger::Init(std::string filename) {
    filename += FormatTimeFilename();
    s_File.open(filename, std::ios::out | std::ios::trunc);
    s_Running = true;
    s_Thread = std::thread(Worker);
}

void Logger::Shutdown() {
    {
        std::scoped_lock lock(s_Mutex);
        s_Running = false;
    }
    s_CondVar.notify_all();
    if (s_Thread.joinable())
        s_Thread.join();

    if (s_File.is_open())
        s_File.close();
}

void Logger::Flush() {
    // Give the worker thread time to process the queue
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void Logger::WriteAsync(LogLevel level, const std::string& msg,
                        const std::source_location& loc) {

    if (!s_Running.load(std::memory_order_acquire)) {
        return;
    }
    LogMessage message;
    message.level = level;
    message.text = msg;
    message.file = std::filesystem::path(loc.file_name()).filename().string();
    message.line = loc.line();
    message.timestamp = std::chrono::system_clock::now();

    Enqueue(std::move(message));
}

void Logger::Enqueue(LogMessage&& message) {
    {
        std::scoped_lock lock(s_Mutex);
        s_Queue.push(std::move(message));
    }
    s_CondVar.notify_one();
}

void Logger::Worker() {
    while (true) {
        std::unique_lock lock(s_Mutex);
        s_CondVar.wait(lock, [] {
            return !s_Queue.empty() ||
                   !s_Running.load(std::memory_order_relaxed);
        });

        while (!s_Queue.empty()) {
            auto msg = std::move(s_Queue.front());
            s_Queue.pop();
            lock.unlock();
            FlushMessage(msg);
            lock.lock();
        }

        if (!s_Running.load(std::memory_order_relaxed) && s_Queue.empty()) {
            break;
        }
    }
}

void Logger::FlushMessage(const LogMessage& msg) {
    // Time formatting with std::chrono
    using namespace std::chrono;
    std::string timeStamp = FormatTimeLog(msg.timestamp);

    // Level + color
    auto idx = static_cast<size_t>(msg.level);
    std::string_view levelStr = kLevelStrs[idx];

    std::string line = std::format("[{}] [{}] {} ({}:{})", timeStamp, levelStr,
                                   msg.text, msg.file, msg.line);

    // Console
    std::cout << line << "\n";

    // File
    if (s_File.is_open()) {
        s_File << line << '\n';
        s_File.flush();
    }
}
} // namespace Utils