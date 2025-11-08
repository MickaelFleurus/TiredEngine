#pragma once
#include "engine/utils/Logger.h"
#include <cstdio>
#include <cstdlib>

// Optional: platform-specific debug break
#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#define DEBUG_BREAK() __builtin_trap()
#else
#define DEBUG_BREAK() ((void)0)
#endif

// --------------------------------------------------------
// Core Engine Assert (Debug only)
// --------------------------------------------------------
#ifndef NDEBUG

#define ENGINE_ASSERT(expr, msg, ...)                                          \
    do {                                                                       \
        if (!(expr)) {                                                         \
            LOG_FATAL("[ASSERTION FAILED] Expression: {} | " msg, #expr,       \
                      ##__VA_ARGS__);                                          \
            Utils::Logger::Flush();                                            \
            DEBUG_BREAK();                                                     \
            std::abort();                                                      \
        }                                                                      \
    } while (0)

#else
#define ENGINE_ASSERT(expr, msg, ...) ((void)0)
#endif

// --------------------------------------------------------
// Verify - Always runs, even in release
// --------------------------------------------------------
#define ENGINE_VERIFY(expr, msg, ...)                                          \
    do {                                                                       \
        if (!(expr)) {                                                         \
            LOG_FATAL("[VERIFY FAILED] Expression: {} | " msg, #expr,          \
                      ##__VA_ARGS__);                                          \
            Utils::Logger::Flush();                                            \
            std::abort();                                                      \
        }                                                                      \
    } while (0)
